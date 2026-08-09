# CognixVision IoT — Project Roadmap

## Purpose

This roadmap tracks the actual implementation state of the CognixVision IoT learning project. A planned item is not considered complete until implementation, testing, and documentation are complete.

## Milestones

| Milestone | Area | Status |
|---|---|---|
| 01 | ESP32 Wi-Fi provisioning | **Complete** |
| 02 | Private IoT Certificate Authority | **Complete** |
| 03 | Device key pair and CSR | **In progress — key generation/persistence verified; CSR next** |
| 04 | Device certificate issuance | Planned |
| 05 | Certificate installation and storage on ESP32 | Planned |
| 06 | MQTT communication | Planned |
| 07 | mTLS between device and broker/backend | Planned |
| 08 | Ultrasonic sensor telemetry | Planned |
| 09 | Spring Boot device ingestion | Planned |
| 10 | Device identity and database/business validation | Planned |
| 11 | Certificate lifecycle and rotation | Planned |
| 12 | Production-style device provisioning/recovery | Planned |

## Current architecture

```text
ESP32
  |
  ├── Wi-Fi provisioning
  │
  ├── Device RSA-2048 private key 🔐
  │      generated once on ESP32
  │      persisted in NVS
  │
  └── Device identity marker
         persisted in NVS
```

The Root CA now exists as the project's trust anchor. The ESP32 has a persistent device key identity. CSR generation, device certificates, MQTT, mTLS, and backend integration are still future steps.

## Milestone 01 recap

Implemented and verified:

- Arduino IDE / C++ firmware
- Serial output
- Wi-Fi connection
- DHCP IP assignment
- WiFiManager configuration
- BOOT/GPIO0 5-second long press
- temporary `ESP32-Setup` AP
- WiFiManager configuration portal
- persisted Wi-Fi credentials
- 10-second connection timeout

## Milestone 02 recap

Implemented and verified:

- local CognixVision Root CA
- 4096-bit RSA CA private key
- self-signed Root CA certificate
- `CA:TRUE` basic constraint
- certificate inspection with OpenSSL
- certificate verification with OpenSSL

The Root CA private key remains on the CA/provisioning workstation and is not installed on the ESP32.

## Milestone 03 current state

The ESP32 now:

- generates an RSA-2048 device key pair locally
- stores the device private key in NVS as a prototype persistence mechanism
- stores a one-byte identity marker
- detects an existing identity after reset
- reuses the existing identity instead of generating a new key
- keeps the private key out of Serial output

The reboot test has passed.

### Next implementation step

Generate an X.509 CSR on the ESP32 using the persisted device private key. The CSR will contain the public key and the lab device identity and will be printed to Serial for CA-side inspection.

## Learning rule

Only introduce the next concept after the current concept has been implemented, tested, documented, and understood.
