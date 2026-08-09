# Cognivison IoT Services

Hands-on IoT platform project built around an ESP32 device, Wi-Fi provisioning, MQTT, Spring Boot services, and mutual TLS/device identity.

## Project goals

- Learn ESP32/embedded development using Arduino C++.
- Build a real device-to-backend communication flow.
- Implement configurable Wi-Fi provisioning and recovery.
- Explore MQTT for IoT telemetry and commands.
- Implement device identity and mTLS.
- Build backend services with Spring Boot.
- Document architecture and engineering decisions as the project evolves.

## Current status

### Completed

- ESP32 DevKit configured in Arduino IDE.
- Serial communication verified at 115200 baud.
- ESP32 connected to Wi-Fi and obtained a DHCP address.
- Basic HTTP endpoint tested from a browser.
- WiFiManager added for configurable Wi-Fi.
- BOOT button verified as GPIO0.
- 5-second BOOT long press implemented as Wi-Fi provisioning trigger.
- ESP32-Setup temporary access point and configuration portal verified.
- New Wi-Fi credentials successfully applied and persisted.
- Wi-Fi connection timeout configured to 10 seconds.

### Next

- Improve Wi-Fi provisioning/reconnection state handling.
- Read ultrasonic sensor data continuously.
- Add device telemetry.
- Introduce MQTT.
- Build Spring Boot MQTT/backend integration.
- Explore device certificates, CSR generation, CA hierarchy, and mTLS.

## Repository structure

```text
firmware/     ESP32 Arduino C++ firmware
docs/         Architecture, decisions, setup and experiments
backend/      Backend services (as introduced)
```

## Documentation

- [Architecture](docs/architecture/overview.md)
- [Hardware setup](docs/setup/hardware.md)
- [Wi-Fi provisioning](docs/decisions/002-wifi-provisioning.md)
- [BOOT provisioning](docs/decisions/003-boot-button-provisioning.md)
