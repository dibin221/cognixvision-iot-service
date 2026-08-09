#include <WiFi.h>
#include <WiFiManager.h>

// ESP32 DevKit BOOT button is connected to GPIO0.
#define BOOT_BUTTON 0

// A continuous 5-second BOOT press enters Wi-Fi provisioning mode.
#define BOOT_HOLD_TIME 5000

WiFiManager wifiManager;

unsigned long bootPressStart = 0;
bool bootPressed = false;
bool provisioningStarted = false;

void setup() {
  Serial.begin(115200);

  pinMode(BOOT_BUTTON, INPUT_PULLUP);

  // Do not allow an individual Wi-Fi connection attempt to wait indefinitely.
  wifiManager.setConnectTimeout(10);

  Serial.println();
  Serial.println("ESP32 starting...");

  // Normal startup: connect using credentials persisted by WiFiManager.
  if (!wifiManager.autoConnect("ESP32-Setup")) {
    Serial.println("Wi-Fi connection failed.");
    ESP.restart();
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // GPIO0 is LOW while the BOOT button is pressed.
  if (digitalRead(BOOT_BUTTON) == LOW) {

    if (!bootPressed) {
      bootPressed = true;
      bootPressStart = millis();

      Serial.println("BOOT button pressed...");
    }

    // Enter provisioning mode only after a continuous 5-second press.
    if (!provisioningStarted &&
        millis() - bootPressStart >= BOOT_HOLD_TIME) {

      provisioningStarted = true;

      Serial.println();
      Serial.println("BOOT held for 5 seconds!");
      Serial.println("Starting Wi-Fi configuration...");

      // Start the temporary ESP32-Setup AP and configuration portal.
      // WiFiManager persists the credentials after successful configuration.
      wifiManager.startConfigPortal("ESP32-Setup");

      Serial.println("Wi-Fi configuration completed.");
      Serial.print("New IP address: ");
      Serial.println(WiFi.localIP());
    }

  } else {
    // Button released; reset long-press tracking.
    if (bootPressed) {
      Serial.println("BOOT button released.");
    }

    bootPressed = false;
    bootPressStart = 0;
  }

  delay(50);
}
