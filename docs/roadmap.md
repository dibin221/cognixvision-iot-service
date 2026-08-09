# CognixVision IoT — Project Roadmap

## Purpose

This roadmap tracks the actual implementation state of the CognixVision IoT learning project. A planned item is not considered complete until implementation, testing, and documentation are complete.

## Milestones

| Milestone | Area | Status |
|---|---|---|
| 01 | ESP32 Wi-Fi provisioning | **Complete** |
| 02 | Private IoT Certificate Authority | **Next** |
| 03 | Device key pair and CSR | Planned |
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
  | Wi-Fi
  v
Local Wi-Fi network
  |
  v
ESP32 HTTP endpoint
```

This is the architecture that currently exists. MQTT, certificates, mTLS, and backend integration are future work.

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

## Milestone 02 objective

Create a **local private IoT Root CA** for the project and understand its role before generating device certificates.

The milestone will document:

- what a CA is
- root CA and trust model
- CA private key vs CA certificate
- how a CA signs a certificate
- where generated artifacts live
- who generates and consumes each artifact
- who trusts whom
- how certificate verification works without contacting the CA for every connection

## Learning rule

Only introduce the next concept after the current concept has been implemented, tested, documented, and understood.
