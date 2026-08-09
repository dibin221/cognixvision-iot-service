# Hardware Setup

## Hardware

Current hardware:

- ESP32 DevKit
- Breadboard
- Ultrasonic distance sensor
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

The project uses a 5-second BOOT long press as the Wi-Fi provisioning trigger.

## Current network test

The ESP32 has successfully connected to a phone hotspot and received a DHCP address. A browser-based HTTP endpoint was also tested successfully.

## Next hardware milestone

Connect the ultrasonic sensor and continuously publish measured distance from the ESP32.
