# Milestone 04 — Device Certificate Provisioning

## Objective

Design and implement a production-inspired device provisioning interface that allows an operator to export a CSR from the ESP32, have it signed externally by the CognixVision CA, and later upload the resulting device certificate back to the ESP32.

This milestone separates **key creation**, **CSR export**, and **certificate installation** instead of generating or printing a CSR automatically on every boot.

## Previous milestone recap

Milestone 03 established the device cryptographic identity:

- RSA-2048 device key generated locally on the ESP32.
- Private key persisted in NVS and reused across reset.
- CSR generated using the persisted private key.
- CSR verified successfully with OpenSSL.
- The device private key never leaves the device.

Milestone 04 turns the laboratory Serial Monitor CSR workflow into an explicit provisioning interface.

## Why we are doing this

A production-inspired IoT device needs an enrollment workflow that an operator can understand and repeat without opening firmware code or copying cryptographic material manually from a serial console.

The intended workflow is:

```text
ESP32
  |
  | Generate CSR using existing private key
  v
CSR export interface
  |
  | download CSR
  v
Operator / CA workstation
  |
  | sign CSR with CA
  v
Device certificate
  |
  | upload certificate
  v
ESP32 certificate validation
  |
  v
Persistent certificate storage
```

## Security model

The device private key is never exported, displayed, uploaded, or sent to the CA.

```text
Private key  -> stays on ESP32
Public key   -> represented in CSR
CSR          -> may leave ESP32
Certificate  -> may be uploaded to ESP32
CA private key -> stays on CA workstation
```

The certificate upload implementation must validate that the certificate corresponds to the device's existing private key before storing it.

## Implemented provisioning interface

The current firmware exposes the local provisioning page at:

```text
http://<device-ip>/provision
```

The implemented page currently contains:

```text
Device Identity
Device ID: esp32-device-001
Key algorithm: RSA-2048
Private key: stored on device

CSR
Generate a CSR using the existing device private key.
Download CSR

Certificate
Certificate upload will be implemented in the next provisioning step.
```

The browser can request the CSR through:

```text
GET /api/provision/csr
```

and receives a PEM-encoded CSR as a downloadable artifact named:

```text
esp32-device-001.csr
```

Certificate upload remains intentionally unimplemented at this stage.

## Provisioning states

The intended state model is:

```text
KEY_NOT_CREATED
      |
      v
KEY_CREATED
      |
      v
CSR_AVAILABLE
      |
      v
CERTIFICATE_PENDING
      |
      v
CERTIFICATE_INSTALLED
      |
      v
PROVISIONED
```

The current tested device has reached the `CSR_AVAILABLE` stage. It has not yet reached `CERTIFICATE_INSTALLED`.

## CSR export implementation

The device loads the persisted private key from NVS and uses mbed TLS to generate and sign the CSR. The CSR contains the device public key and the configured device common name. The private key is not included in the HTTP response. The firmware explicitly clears the temporary private-key string after parsing and frees the mbed TLS key context after CSR generation. fileciteturn56file4L763-L799 fileciteturn56file5L903-L1008

The HTTP handler does not perform the cryptographic operation directly on Arduino's `loopTask`. CSR generation is run on a dedicated FreeRTOS task with a larger stack. This was added after the first interface implementation triggered an ESP32 `Stack canary watchpoint triggered (loopTask)` panic while generating the CSR.

The dedicated-task approach prevents the relatively stack-heavy mbed TLS operation from exhausting the Arduino loop task stack. The task calls the existing `generateCSR()` implementation after a forward declaration, then returns the generated PEM CSR to the HTTP handler. fileciteturn56file4L763-L799

## Components

- ESP32 firmware
- Arduino Wi-Fi stack
- WiFiManager
- ESP32 WebServer/HTTP interface
- FreeRTOS task for CSR generation
- Mbed TLS X.509 CSR functionality
- NVS for persistent private-key storage
- External OpenSSL-based CA workstation

## Endpoints

```text
GET  /provision
GET  /api/provision/csr
POST /api/provision/certificate   (planned; not implemented)
```

`GET /provision` displays the local provisioning interface. The current page exposes the CSR download operation and explicitly indicates that certificate upload is still a future step. fileciteturn56file5L1011-L1041

`GET /api/provision/csr` generates a CSR from the current device identity and returns it as a downloadable PEM artifact.

`POST /api/provision/certificate` will be added later and will validate and persist the signed certificate.

## Certificate validation requirements for the later upload step

Before storing an uploaded certificate, the ESP32 must verify at least:

1. Certificate syntax is valid.
2. Certificate is signed by the configured CognixVision trust anchor.
3. Certificate public key matches the device private key.
4. Certificate is not a CA certificate (`CA:FALSE`).
5. Required device authentication usage is present.
6. Certificate validity dates are acceptable.

Only a certificate passing these checks should become the active device certificate.

## Generated artifacts

### CSR

Generated by the ESP32 from the persisted device private key.

Consumer: CA/provisioning workstation.

Storage: operator workstation after browser download; not required to persist on the ESP32.

Actual test result: **CSR download succeeded through the browser.**

### Device certificate

Generated by the external CA after signing the CSR.

Consumer: ESP32 device provisioning workflow.

Storage: ESP32 NVS after successful validation. This is planned and has not yet been implemented.

## Files

Firmware implementation is represented by the complete ESP32 sketch used for this milestone. The repository firmware location remains:

```text
firmware/esp32/
```

The CSR itself is a generated provisioning artifact and must not be committed to the repository as a device credential.

## Security considerations

- Never expose the private key through HTTP, Serial, or download.
- Do not accept a certificate solely because it parses successfully.
- Do not replace a valid installed certificate with an invalid upload.
- Keep the CA private key off the ESP32.
- The provisioning interface is currently intended for a controlled local network; authentication and transport protection will need to be addressed before real production deployment.
- The current NVS private-key storage is a prototype persistence mechanism; production hardware should use hardware-backed key protection where available.

## Testing procedure — CSR portion

1. Boot the ESP32.
2. Confirm an existing device key is reused rather than regenerated.
3. Open the provisioning interface from a browser on the same network.
4. Request CSR generation/download.
5. Save the response as `esp32-device-001.csr`.
6. Confirm the browser successfully downloads the CSR.
7. Verify the downloaded CSR externally with OpenSSL.
8. Confirm its public key corresponds to the device identity.
9. Confirm the private key is never exposed by the interface.

## Expected result

The operator can obtain a PEM CSR directly from the device through the provisioning interface without accessing or exporting the device private key.

## Actual result

**CSR export interface — IMPLEMENTED AND TESTED.**

Observed successful behavior:

- ESP32 boots with the existing persisted device identity.
- The device does not regenerate the RSA-2048 key on reset.
- The provisioning HTTP server starts after Wi-Fi connection.
- The `/provision` page is reachable from the browser.
- Selecting the CSR download operation successfully generates the CSR.
- The browser successfully downloads the CSR artifact.

### Problem encountered during implementation

The first CSR download implementation generated the CSR directly from Arduino's `loopTask`. This caused:

```text
Guru Meditation Error: Core 1 panic'ed
Debug exception reason: Stack canary watchpoint triggered (loopTask)
```

The failure occurred while loading the persisted private key and entering mbed TLS CSR generation.

### Resolution

CSR generation was moved to a dedicated FreeRTOS task with a larger stack. A forward declaration for `generateCSR()` was also added so the task function is declared in a valid order for the C++ compiler. The corrected firmware compiled and the CSR download subsequently succeeded.

### Remaining verification

The downloaded CSR still needs its external OpenSSL verification recorded as part of the final CSR-export acceptance check. Certificate upload must not begin until that verification is recorded.

## Decisions made

- CSR generation is an explicit provisioning operation rather than an automatic boot operation.
- Private key remains device-resident.
- CSR is an exportable artifact.
- Certificate is an importable artifact.
- Certificate upload will be validated against the existing device key before persistence.
- CSR export is implemented and tested before certificate upload.
- Stack-heavy cryptographic work is isolated from Arduino's `loopTask` using a dedicated FreeRTOS task.
- The CSR itself is not persisted on the ESP32; it is generated on demand and exported to the operator.

## What changed from Milestone 03

Milestone 03 proved that the cryptographic identity and CSR can be generated correctly. Milestone 04 now adds an operator-facing HTTP provisioning workflow around that identity and provides a browser-downloadable CSR artifact.

## Important concepts learned

- **Provisioning:** the process of preparing a device with the identity and credentials it needs to operate securely.
- **CSR:** a certificate signing request containing the device identity and public key, signed using the corresponding private key.
- **Certificate installation:** accepting a CA-signed certificate only after verifying that it belongs to the device's existing key.
- **Dedicated task:** a separate FreeRTOS execution context with its own stack, useful when an operation needs more stack space than the Arduino loop task can safely provide.

## What you should now understand

The device creates and protects its own private key. The operator can now request a CSR through the device's provisioning page. The ESP32 uses its existing private key to sign the CSR, while only the CSR leaves the device. The CA can sign that CSR externally. The resulting certificate will later be returned to the device and validated before installation.

## Next step

**Verify the downloaded CSR with OpenSSL.** Record the verification result in this milestone. Only after that verification is documented should we implement certificate upload and certificate storage on the ESP32.
