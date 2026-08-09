# Decision 002: Configurable Wi-Fi Provisioning

## Problem

The ESP32 must be able to change Wi-Fi networks without reflashing firmware or hard-coding new credentials.

## Decision

Use WiFiManager to provide a temporary configuration access point and web portal.

Normal operation uses persisted Wi-Fi credentials. A deliberate provisioning action starts the configuration portal.

## Provisioning trigger

Use the existing ESP32 DevKit `BOOT` button on GPIO0. A 5-second long press triggers provisioning.

No additional physical button is required.

## Flow

```text
Normal startup
    |
    v
Saved Wi-Fi credentials
    |
    v
Connect to Wi-Fi
    |
    v
Normal operation
```

Provisioning flow:

```text
BOOT held for 5 seconds
    |
    v
Start ESP32-Setup AP
    |
    v
Configuration portal at 192.168.4.1
    |
    v
Select Wi-Fi and provide credentials
    |
    v
Connect to new Wi-Fi
    |
    v
Persist credentials
```

## Verification

The provisioning flow was successfully tested. The ESP32 created `ESP32-Setup`, exposed the portal at `192.168.4.1`, connected to the selected phone hotspot, and received `10.73.214.139` through DHCP.

One initial connection attempt failed and a subsequent retry succeeded.

## Timeout

A 10-second Wi-Fi connection timeout was selected so a connection attempt does not wait indefinitely.

```cpp
wifiManager.setConnectTimeout(10);
```

## Future improvements

- Separate provisioning state from normal connected state.
- Add explicit retry/backoff behaviour.
- Define failure and recovery behaviour when saved Wi-Fi is unavailable.
- Add a deliberate factory-reset mechanism separately from Wi-Fi provisioning.
