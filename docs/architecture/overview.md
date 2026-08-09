# Architecture Overview

## Current stage

The project is currently focused on ESP32 network connectivity and Wi-Fi provisioning.

Only the device-side Wi-Fi functionality has been implemented so far. MQTT, backend services, sensor telemetry, device certificates, and mTLS are planned but not yet implemented.

## Current architecture

```text
+-------------------+
| ESP32 DevKit      |
| Arduino C++       |
|                   |
| Wi-FiManager      |
+---------+---------+
          |
          | Wi-Fi
          v
+-------------------+
| Wi-Fi Access Point|
| / phone hotspot   |
+-------------------+
```

The ESP32 normally operates as a Wi-Fi station and connects using persisted credentials.

## Wi-Fi provisioning architecture

A 5-second hold of the physical `BOOT` button triggers provisioning.

```text
                  BOOT held 5 sec
                         |
                         v
                    +---------+
                    |  ESP32  |
                    +----+----+
                         |
                         | creates temporary AP
                         v
                  +--------------+
                  | ESP32-Setup  |
                  | 192.168.4.1  |
                  +------+-------+
                         |
                         | configuration portal
                         v
                    Phone/Laptop
                         |
                         | select Wi-Fi
                         v
                    New Wi-Fi AP
                         |
                         v
                    ESP32 connects
                         |
                         v
                  Credentials persist
```

## Verified behaviour

The device successfully:

- connected to a phone hotspot;
- received a DHCP address;
- entered provisioning mode using the BOOT button;
- created `ESP32-Setup`;
- exposed the WiFiManager portal at `192.168.4.1`;
- connected to the newly selected Wi-Fi network; and
- persisted the new Wi-Fi configuration.

## Planned architecture

```text
ESP32
  |
  | Wi-Fi + MQTT/TLS
  v
MQTT Broker
  |
  v
Spring Boot Backend
  |
  +--> Device registry
  +--> Telemetry processing
  +--> Device commands
```

Device identity, certificates, CSR/CA workflow, and mutual TLS will be introduced only in later milestones.
