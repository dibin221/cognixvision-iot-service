# Milestone 03 — Device Identity

## Status
In progress — **device key generation and reboot persistence verified; CSR generation is the next implementation step**

## Previous milestone recap
Milestone 02 created and verified the CognixVision Root CA:

```text
CognixVision Root CA
├── Root CA private key   🔐
└── Root CA certificate   📜
```

The Root CA certificate is self-signed, contains the CA public key, has `CA:TRUE`, and was successfully verified with OpenSSL.

Milestone 03 connects that trust anchor to an individual IoT device.

## Objective
Establish a **production-inspired cryptographic identity model** in which each ESP32 generates and retains its own private key. The private key must never be exported to the CA, backend, Git repository, or operator workstation.

The device generates its key pair locally, retains the private key across reboots, creates a Certificate Signing Request (CSR), and provides only the CSR to the CA/provisioning process. The CA will sign the CSR and return the device certificate.

## Why we are doing this
An IoT platform should not identify a device only by a serial number or IP address. A cryptographic device identity provides a way to prove possession of a secret during authentication.

More importantly, the private key should be generated where it will be used. Generating a device private key on an operator workstation and then transferring it to the ESP32 would create an unnecessary copy of the secret and would not represent the production-inspired architecture we want to learn.

The device key is therefore generated **once**, during identity initialization, and reused on subsequent boots. A normal reset must not generate a new identity.

This will eventually support an mTLS flow such as:

```text
ESP32                              MQTT / Backend
  │                                      │
  │ device certificate                  │
  │ + proves possession of              │
  │   device private key                │
  ├──────── TLS / mTLS ─────────────────►│
  │                                      │
  │                         trusts CognixVision Root CA
```

## Architecture

The intended certificate chain is:

```text
                    CognixVision Root CA
                           │
                           │ signs CSR
                           ▼
                    Device Certificate
                           │
                           │ contains
                           ▼
                    Device Public Key

ESP32 keeps separately:
                    Device Private Key 🔐
```

The CA private key remains only on the CA side.

### Current implemented provisioning/identity flow

```text
ESP32 boot
   │
   ▼
Check NVS identity marker
   │
   ├── identity exists ──► reuse existing device key
   │                         │
   │                         └── continue boot
   │
   └── identity absent ──► generate RSA-2048 key pair
                              │
                              ├── store private key in NVS
                              ├── store identity marker
                              └── continue boot
```

### Next certificate provisioning flow

```text
ESP32
  │
  │ 1. Load persisted private key
  │
  │ 2. Generate CSR using private key
  │
  ├──────── CSR containing public key ────────► Provisioning / CA
  │                                             │
  │                                             │ 3. Validate request
  │                                             │ 4. CA signs certificate
  │                                             │
  │◄──────────── Device certificate ───────────┤
  │
  │ 5. Store certificate
  │
  ▼
ESP32 now has its cryptographic identity
```

## Important terminology — simple first

### Device private key
A secret key generated specifically for one device. It remains on the device and is used to prove possession of the device identity.

### Device public key
The non-secret counterpart of the device private key. It can be included in a CSR and certificate.

### CSR (Certificate Signing Request)
A request containing device identity information and the device public key. The CSR is signed using the device private key so the CA can verify that the requester possesses the corresponding private key.

### Device certificate
A certificate signed by the Root CA that binds the device identity to its public key.

### Bootstrap/provisioning identity
The mechanism used to establish enough initial trust for a device to request its long-term certificate. We will design this explicitly rather than assuming the device is already trusted.

### Key distinction

```text
Private key  → secret → proves possession / signs
Public key   → shareable → verifies signatures
CSR          → request containing public key + identity, signed by device key
Certificate  → CA-signed identity + public key
```

## Planned artifact lifecycle

```text
1. ESP32 generates device private/public key pair
             │
             │ private key stays on ESP32
             ▼
2. ESP32 generates CSR
             │
             │ CSR contains public key
             ▼
3. CSR reaches provisioning/CA process
             │
             ▼
4. CA validates request
             │
             ▼
5. CA signs device certificate
             │
             ▼
6. Certificate is returned to ESP32
             │
             ▼
7. ESP32 uses certificate + private key for future mTLS
```

## Artifact ownership and trust

| Artifact | Generated by | Stored/used by | Secret? |
|---|---|---|---|
| Root CA private key | CA operator/provisioning environment | CA environment | **Yes** |
| Root CA certificate | CA operator/provisioning environment | Trusting systems | No |
| Device private key | **ESP32** | **ESP32 only** | **Yes** |
| Device public key | ESP32 / derived from device key | CSR/certificate | No |
| Device CSR | **ESP32** | Provisioning/CA | No, but contains public identity material |
| Device certificate | CA | ESP32 / verifier | No |

## Security requirements

1. The Root CA private key must never be installed on the ESP32.
2. The device private key must be generated on the ESP32 for the production-inspired design.
3. The device private key must never be exported to the backend, CA, Git, or operator workstation.
4. The device should prove possession of its private key rather than transmitting the private key itself.
5. Device certificates can be distributed to the device and to systems that need to authenticate it.
6. Production devices should use protected key storage where available; this lab currently uses NVS as a prototype persistence mechanism and will evaluate stronger ESP32-specific protection before production use.
7. The provisioning/bootstrap mechanism must be treated as a separate trust decision and documented before production use.

## Current implementation

### Firmware environment

- Platform: ESP32 DevKit
- Arduino IDE
- ESP32 Arduino Core: **3.3.11**
- Language: C++
- Cryptography: ESP32/Arduino bundled Mbed TLS APIs
- Persistent storage: ESP32 NVS through the Arduino `Preferences` API

### Implemented behavior

The firmware now performs the following during startup:

1. Open the `device` NVS namespace in read-only mode.
2. Look for a one-byte `identity` marker.
3. If the marker exists with value `1`, treat the device identity as already initialized and **do not generate a new key**.
4. If the marker is absent, generate an RSA-2048 key pair on the ESP32.
5. Export the private key to PEM in memory.
6. Persist the private key in NVS.
7. Persist the one-byte identity marker.
8. Verify that the identity marker can be read back.
9. Clear the temporary private-key buffer and release the Mbed TLS structures.
10. Continue with the existing Wi-Fi connection/provisioning flow.

The firmware source is stored at:

```text
firmware/esp32/wifi_provisioning/wifi_provisioning.ino
```

### Important prototype limitation

The current implementation stores the PEM private key as ordinary NVS application data. This proves the identity lifecycle, but it is **not yet the final production security design**. Before production use, the project should evaluate ESP32 secure storage/hardware-backed key facilities and flash-encryption/secure-boot configuration as appropriate for the target hardware.

## Testing performed

### Test 1 — NVS persistence in isolation

A minimal NVS test stored a one-byte identity marker and then read it after reset.

Observed result:

```text
Bytes read: 1
Marker value: 1

Identity marker EXISTS.
NVS data survived the reboot.
```

This established that NVS persistence survives an ESP32 reset.

### Test 2 — Device key persistence

The integrated firmware was flashed to the ESP32. On the first identity initialization, the device generated an RSA-2048 key and persisted the private key plus identity marker.

After pressing RESET, the observed output was:

```text
ESP32 starting...

Checking device identity...
Device identity already exists.
Existing device key will be reused.
```

The RSA generation message did **not** appear on the subsequent boot.

### Actual result

**PASS** — the device identity is no longer regenerated on normal reboot/reset.

The verified lifecycle is:

```text
First initialization
  → generate RSA-2048 key
  → persist identity

Subsequent boot/reset
  → detect identity
  → reuse existing identity
  → do not generate a new key
```

## Generated artifacts

### Device private key

- Generated by: ESP32
- Stored in: ESP32 NVS (`device` namespace, `private_key` entry)
- Consumed by: future CSR generation and TLS/mTLS
- Secret: **Yes**
- Not printed to Serial
- Not transmitted to the CA/backend

### Device public key

- Generated as part of the RSA key pair
- Derived from the private key when needed
- Will be included in the CSR
- Secret: No

### Device CSR

- **Not generated yet**
- Next artifact to implement
- Will be generated on the ESP32 using the persisted private key
- Will contain the device public key and requested subject identity

## Device identity naming

The lab uses the logical identifier:

```text
esp32-device-001
```

This is an application-level lab identifier. It is not being treated as an immutable hardware identifier.

For the first CSR implementation, this identifier will be used as the requested certificate subject CN. A later provisioning design can introduce a stronger device identifier/attestation mechanism.

## Decisions made

### Production-inspired device key generation
The device private key is generated on the ESP32 rather than on the workstation. This better models a real device-provisioning architecture and avoids unnecessary exposure of the private key.

### Generate once, reuse across reboot
A normal boot/reset must not create a new device identity. Persistent NVS state is used to detect whether the identity has already been initialized.

### Separate key per device
Each device gets its own unique key pair. Compromise of one device key should not expose the CA private key or automatically compromise other devices.

### RSA Root CA, RSA device key for the current lab step
The Root CA uses RSA 4096. The current device experiment uses RSA 2048 because it is supported by the tested ESP32/Mbed TLS environment and gives us a straightforward end-to-end learning path. Algorithm selection remains an architectural decision to revisit before production deployment.

### NVS for initial prototype persistence
NVS was selected to demonstrate persistent identity lifecycle behavior. It is not being declared sufficient as a production-grade private-key protection mechanism.

## Problems encountered / design corrections

### Problem: device key regenerated on every reset
The first implementation used a boolean identity marker and appeared to lose the marker. A standalone NVS test then proved that NVS itself persists data correctly.

The implementation was simplified to use a one-byte identity marker stored with `putBytes()` and read with `getBytes()`. The integrated firmware was then tested successfully across reset.

### Design correction: workstation-generated device key
The initial plan proposed generating `esp32-device-001.key` on the workstation. That was corrected before implementation because the private key should be generated and retained by the device in the production-inspired architecture.

## What changed from the previous milestone

Milestone 02 established the Root CA trust anchor. Milestone 03 now has a working device-side identity lifecycle:

```text
Root CA
  │
  │ will eventually sign
  ▼
Device CSR
  ▲
  │ generated using
  │
ESP32 device private key
```

The ESP32 now generates its device key once and retains it across normal resets.

## What you should now understand

- The Root CA has its own key pair; the device has a completely separate key pair.
- The ESP32 generates its device private key locally.
- The device private key is generated **once**, not on every boot.
- NVS provides persistence across reset; it does not by itself make the stored private key hardware-protected.
- The private key should never be sent to the CA or backend.
- The public key can be shared through the CSR.
- The CSR proves that the requester possesses the corresponding private key because the CSR is signed by that key.
- The CA signs the CSR and returns a certificate.
- The resulting certificate and private key will later allow the ESP32 to authenticate using mTLS.

## Next step — CSR generation

We will now implement **CSR generation on the ESP32**.

The CSR implementation will:

1. Load the persisted device private key from NVS.
2. Parse it into an Mbed TLS key context.
3. Create an X.509 CSR using the device private key.
4. Set the requested subject to the lab device identity (`CN=esp32-device-001`).
5. Sign the CSR with the device private key.
6. Produce the CSR in PEM format.
7. Print the CSR to Serial for this lab so it can be copied to the CA/provisioning workstation.
8. **Never print or transmit the private key.**

The CSR will then be inspected and verified before we proceed to CA signing.
