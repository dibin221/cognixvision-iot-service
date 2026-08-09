# Architecture Overview

## Current stage

The project is being built incrementally. The current focus is the ESP32 device and its network connectivity. MQTT, backend services, and device mTLS will be introduced in later stages.

## Current architecture

```text
+-------------------+
| ESP32 DevKit      |
| Arduino C++       |
|                   |
| Ultrasonic sensor |
+---------+---------+
          |
          | Wi-Fi
          v
+-------------------+
| Wi-Fi Access Point|
| / phone hotspot   |
+-------------------+
```

The ESP32 can also temporarily operate as a Wi-Fi Access Point during provisioning:

```text
Phone / Laptop
      |
      | connects to
      v
ESP32-Setup
      |
      v
192.168.4.1
      |
      v
WiFiManager web portal
```

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

Device identity and mutual TLS will be added as a later milestone.
