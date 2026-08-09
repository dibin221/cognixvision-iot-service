# Cognivision IoT Services

Hands-on IoT platform project built incrementally around an ESP32 device and a cloud/backend platform.

## Current focus

The project is currently at the **Wi-Fi configuration stage**. No MQTT, sensor telemetry, backend integration, certificates, or mTLS has been implemented yet.

### Implemented

- ESP32 DevKit configured in Arduino IDE using C++.
- Serial communication verified at 115200 baud.
- ESP32 connected to a Wi-Fi network and received a DHCP address.
- WiFiManager added for configurable Wi-Fi.
- BOOT button verified as GPIO0.
- 5-second BOOT long press implemented as a Wi-Fi provisioning trigger.
- Temporary `ESP32-Setup` access point and WiFiManager configuration portal verified.
- New Wi-Fi credentials successfully applied and persisted.
- 10-second Wi-Fi connection timeout configured.

## Current Wi-Fi flow

```text
ESP32 startup
    |
    v
WiFiManager
    |
    +---- saved credentials ----> connect to Wi-Fi
    |
    +---- BOOT held 5 seconds --> ESP32-Setup AP
                                      |
                                      v
                              WiFiManager portal
                                      |
                                      v
                              configure Wi-Fi
                                      |
                                      v
                              connect + persist
```

## Planned stages

1. Complete Wi-Fi recovery/reconnection behaviour.
2. Connect and read the ultrasonic sensor.
3. Generate device telemetry.
4. Introduce MQTT.
5. Build the Spring Boot backend integration.
6. Add device identity, certificates, CSR/CA flow, and mTLS.

## Repository structure

```text
firmware/     ESP32 Arduino C++ firmware
docs/         Architecture, decisions and setup notes
backend/      Reserved for future backend implementation
```

## Documentation

- [Architecture](docs/architecture/overview.md)
- [Hardware setup](docs/setup/hardware.md)
- [Wi-Fi provisioning](docs/decisions/002-wifi-provisioning.md)
- [BOOT provisioning](docs/decisions/003-boot-button-provisioning.md)
