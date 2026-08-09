#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>

#include <mbedtls/pk.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <cstring>

// ESP32 DevKit BOOT button is connected to GPIO0.
#define BOOT_BUTTON 0

// A continuous 5-second BOOT press enters Wi-Fi provisioning mode.
#define BOOT_HOLD_TIME 5000

WiFiManager wifiManager;
Preferences preferences;

unsigned long bootPressStart = 0;
bool bootPressed = false;
bool provisioningStarted = false;

bool deviceIdentityExists() {
  preferences.begin("device", true);

  uint8_t marker = 0;
  size_t bytesRead = preferences.getBytes("identity", &marker, sizeof(marker));

  preferences.end();

  return (bytesRead == 1 && marker == 1);
}

bool generateDeviceKey() {
  Serial.println();
  Serial.println("Generating device RSA key...");

  mbedtls_pk_context key;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctrDrbg;

  mbedtls_pk_init(&key);
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctrDrbg);

  const char *personalization = "cognixvision-device";

  int result = mbedtls_ctr_drbg_seed(
      &ctrDrbg,
      mbedtls_entropy_func,
      &entropy,
      (const unsigned char *)personalization,
      strlen(personalization));

  if (result != 0) {
    Serial.printf("Random generator initialization failed: -0x%04X\n", -result);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_pk_free(&key);
    return false;
  }

  result = mbedtls_pk_setup(
      &key,
      mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));

  if (result != 0) {
    Serial.printf("RSA key setup failed: -0x%04X\n", -result);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_pk_free(&key);
    return false;
  }

  Serial.println("Generating RSA-2048 key pair...");

  result = mbedtls_rsa_gen_key(
      mbedtls_pk_rsa(key),
      mbedtls_ctr_drbg_random,
      &ctrDrbg,
      2048,
      65537);

  if (result != 0) {
    Serial.printf("RSA key generation failed: -0x%04X\n", -result);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_pk_free(&key);
    return false;
  }

  Serial.println("RSA-2048 device key generated.");

  unsigned char privateKey[2048];
  memset(privateKey, 0, sizeof(privateKey));

  result = mbedtls_pk_write_key_pem(
      &key,
      privateKey,
      sizeof(privateKey));

  if (result != 0) {
    Serial.printf("Private key export failed: -0x%04X\n", -result);
    memset(privateKey, 0, sizeof(privateKey));
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_pk_free(&key);
    return false;
  }

  Serial.println("Private key exported.");
  Serial.println("Storing private key in NVS...");

  preferences.begin("device", false);
  size_t privateKeyBytes = preferences.putString("private_key", (char *)privateKey);
  preferences.end();

  Serial.print("Private key bytes written: ");
  Serial.println(privateKeyBytes);

  if (privateKeyBytes == 0) {
    Serial.println("ERROR: Failed to store private key.");
    memset(privateKey, 0, sizeof(privateKey));
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_pk_free(&key);
    return false;
  }

  Serial.println("Storing device identity marker...");

  uint8_t identityMarker = 1;

  preferences.begin("device", false);
  size_t markerBytes = preferences.putBytes(
      "identity",
      &identityMarker,
      sizeof(identityMarker));
  preferences.end();

  Serial.print("Identity marker bytes written: ");
  Serial.println(markerBytes);

  if (markerBytes != 1) {
    Serial.println("ERROR: Failed to store identity marker.");
    memset(privateKey, 0, sizeof(privateKey));
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_pk_free(&key);
    return false;
  }

  if (!deviceIdentityExists()) {
    Serial.println("ERROR: Identity marker verification failed.");
    memset(privateKey, 0, sizeof(privateKey));
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_pk_free(&key);
    return false;
  }

  memset(privateKey, 0, sizeof(privateKey));
  mbedtls_ctr_drbg_free(&ctrDrbg);
  mbedtls_entropy_free(&entropy);
  mbedtls_pk_free(&key);

  Serial.println();
  Serial.println("================================");
  Serial.println("Device identity created.");
  Serial.println("Private key persisted.");
  Serial.println("================================");

  return true;
}

void initializeDeviceIdentity() {
  Serial.println();
  Serial.println("Checking device identity...");

  if (deviceIdentityExists()) {
    Serial.println("Device identity already exists.");
    Serial.println("Existing device key will be reused.");
    return;
  }

  Serial.println("No device identity found.");
  Serial.println("This appears to be the first identity initialization.");

  if (!generateDeviceKey()) {
    Serial.println();
    Serial.println("ERROR: Device identity generation failed.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BOOT_BUTTON, INPUT_PULLUP);

  wifiManager.setConnectTimeout(10);

  Serial.println();
  Serial.println("ESP32 starting...");

  initializeDeviceIdentity();

  if (!wifiManager.autoConnect("ESP32-Setup")) {
    Serial.println("Wi-Fi connection failed.");
    ESP.restart();
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (digitalRead(BOOT_BUTTON) == LOW) {
    if (!bootPressed) {
      bootPressed = true;
      bootPressStart = millis();
      Serial.println("BOOT button pressed...");
    }

    if (!provisioningStarted &&
        millis() - bootPressStart >= BOOT_HOLD_TIME) {
      provisioningStarted = true;

      Serial.println();
      Serial.println("BOOT held for 5 seconds!");
      Serial.println("Starting Wi-Fi configuration...");

      wifiManager.startConfigPortal("ESP32-Setup");

      Serial.println("Wi-Fi configuration completed.");
      Serial.print("New IP address: ");
      Serial.println(WiFi.localIP());
    }
  } else {
    if (bootPressed) {
      Serial.println("BOOT button released.");
    }

    bootPressed = false;
    bootPressStart = 0;
  }

  delay(50);
}
