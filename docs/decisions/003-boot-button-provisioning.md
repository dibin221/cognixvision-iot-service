# Decision 003: BOOT Long Press for Wi-Fi Reconfiguration

## Problem

The device does not have a dedicated Wi-Fi reset/provisioning button. Network configuration must still be recoverable without reflashing the firmware.

## Decision

Use the existing ESP32 DevKit BOOT button as a software-controlled provisioning trigger.

A continuous 5-second press is interpreted as a request to enter Wi-Fi provisioning mode.

## Why BOOT instead of RESET

The RESET/EN button performs a hardware reset. The firmware cannot use a reset long press as a normal application-level input in the same way.

The BOOT button is connected to GPIO0 and can be read by application code.

## Implementation

The firmware continuously monitors GPIO0 in `loop()`.

```text
GPIO0 LOW
   |
   v
Start timer
   |
   v
Remain LOW for >= 5 seconds?
   |
  YES
   |
   v
Start WiFiManager configuration portal
```

The implementation deliberately does **not** call `resetSettings()` when the BOOT button is held. The action means "reconfigure Wi-Fi", not "erase all Wi-Fi settings".

## Verification

The button was first tested independently and produced the expected `BOOT PRESSED` output. The integrated implementation then successfully produced:

```text
BOOT button pressed...
BOOT held for 5 seconds!
Starting Wi-Fi configuration...
```

WiFiManager subsequently created `ESP32-Setup`, exposed the configuration portal at `192.168.4.1`, and connected to the newly selected Wi-Fi network.

## Design principle

The long press represents **Wi-Fi reconfiguration**, not a destructive factory reset. Factory reset should be a separate, deliberately protected operation if it is introduced later.
