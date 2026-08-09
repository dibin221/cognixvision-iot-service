# CognixVision IoT Service

A production-inspired IoT learning project built incrementally around an ESP32 device and a backend platform.

## Documentation-first rule

Documentation is a mandatory part of every milestone. Code, architecture, decisions, tests, generated artifacts, and documentation must remain synchronized.

For every implementation step:

`Understand → Plan → Document → Implement → Test → Inspect → Update documentation → Update roadmap → Commit`

A milestone is complete only when the implementation works, tests have been performed, and the relevant documentation has been updated with the actual result.

## Current status

### Milestone 01 — Wi-Fi provisioning — COMPLETE

The ESP32 can:

- run firmware built with Arduino IDE / C++
- connect to a configured Wi-Fi network using WiFiManager
- persist Wi-Fi credentials
- expose a temporary `ESP32-Setup` access point for provisioning
- open a WiFiManager configuration portal
- use the physical BOOT button (GPIO0) as a 5-second Wi-Fi reconfiguration trigger
- use a 10-second Wi-Fi connection timeout

Verified during the milestone:

- serial communication at 115200 baud
- Wi-Fi connection and DHCP address assignment
- browser access to the ESP32 over the local network
- BOOT/GPIO0 long-press detection
- `ESP32-Setup` AP at `192.168.4.1`
- successful provisioning and reconnection to the configured Wi-Fi

No MQTT, sensor telemetry, certificates, mTLS, or backend device authentication has been implemented yet.

## Next milestone

### Milestone 02 — Private IoT CA

We will first understand and create a local Certificate Authority. Only after that milestone is documented and verified will we move to device key generation, CSR creation, certificate signing, and eventually mTLS.

## Repository structure

```text
docs/
├── roadmap.md
├── glossary.md
├── architecture/
├── milestones/
└── security/

firmware/
└── esp32/

backend/
```

## Source of truth

The repository documentation is the long-term record of the project. When implementation decisions change, existing documentation must be corrected rather than leaving contradictory historical descriptions.
