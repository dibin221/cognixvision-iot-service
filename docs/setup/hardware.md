# Hardware Setup

## Current hardware

- ESP32 DevKit
- Breadboard
- Ultrasonic distance sensor (not integrated yet)
- USB data cable

## ESP32 setup

The board was configured using Arduino IDE and verified over the serial port at 115200 baud.

## BOOT button

The physical button labelled `BOOT` was tested and confirmed to be readable through GPIO0.

Observed behaviour:

```text
BOOT not pressed -> GPIO0 HIGH
BOOT pressed     -> GPIO0 LOW
```

The firmware uses a continuous 5-second BOOT long press as the Wi-Fi provisioning trigger.

## Current network milestone

The ESP32 has successfully connected to a phone hotspot and received a DHCP address.

WiFiManager provisioning was also verified:

```text
BOOT held 5 seconds
        |
        v
ESP32-Setup AP
        |
        v
192.168.4.1 configuration portal
        |
        v
Select Wi-Fi
        |
        v
ESP32 connects to new network
```

The ultrasonic sensor is physically available but has **not yet been connected or programmed**. Sensor integration is the next hardware milestone.
