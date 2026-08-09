# CognixVision IoT — Glossary

Simple explanation first, followed by the technical meaning.

## Private key

**Simple:** A secret digital key that must stay with its owner.

**Technical:** A cryptographic key used to create digital signatures or perform private-key operations. It must be protected and must not be shared.

## Public key

**Simple:** The non-secret half of a key pair that can be shared.

**Technical:** The public counterpart of a private key. Others can use it to verify signatures or perform public-key cryptographic operations.

## Key pair

**Simple:** Two mathematically related keys: one private and one public.

**Technical:** An asymmetric cryptographic key pair consisting of a private key and its corresponding public key.

## CSR — Certificate Signing Request

**Simple:** A request asking a Certificate Authority to issue a certificate for a public key.

**Technical:** A PKCS#10 structure containing a subject identity, a public key, and a proof of possession of the corresponding private key. A CSR is submitted to a CA for signing.

## Certificate

**Simple:** A signed document that binds an identity to a public key.

**Technical:** An X.509 certificate containing a public key, subject/identity information, validity information, issuer information, and a CA signature.

## CA — Certificate Authority

**Simple:** A trusted authority that signs certificates.

**Technical:** An entity that issues and signs X.509 certificates. In this project we will initially operate our own private CA.

## Root CA

**Simple:** The top-level CA that we decide to trust.

**Technical:** A self-signed CA certificate that anchors a certificate trust chain.

## Intermediate CA

**Simple:** A CA below the Root CA that can issue certificates without exposing the Root CA private key.

**Technical:** A subordinate CA certificate signed by a parent CA and used to issue end-entity certificates.

## Trust store

**Simple:** A place where trusted certificates are kept.

**Technical:** A collection of trusted CA certificates used to validate certificate chains.

## mTLS — Mutual TLS

**Simple:** Both sides of a TLS connection prove their identity using certificates.

**Technical:** TLS configured so that the server authenticates the client certificate as well as the client authenticating the server certificate.

## Provisioning

**Simple:** The process of preparing a device so it can securely join and operate in the system.

**Technical:** Device onboarding activities such as configuring connectivity, establishing identity, installing credentials/certificates, and registering the device.

## Bootstrap identity

**Simple:** A temporary or initial identity that allows a device to obtain its normal credentials.

**Technical:** Initial credentials or identity material used during secure device onboarding before the long-term device identity is established.

## Certificate rotation

**Simple:** Replacing an expiring or compromised certificate with a new one.

**Technical:** The controlled lifecycle process of issuing and deploying replacement certificates while maintaining service continuity.

## Revocation

**Simple:** Declaring a certificate no longer trustworthy before it expires.

**Technical:** A mechanism for invalidating a certificate before its natural expiration, commonly represented through CRLs or OCSP in PKI systems.

## DHCP

**Simple:** The network service that gives a device an IP address.

**Technical:** Dynamic Host Configuration Protocol, used to automatically assign network configuration such as an IP address, gateway, and DNS servers.

## Access Point (AP)

**Simple:** A Wi-Fi network that other devices can connect to.

**Technical:** A network interface operating in Wi-Fi access-point mode. During provisioning, the ESP32 creates the temporary `ESP32-Setup` AP.

## Wi-Fi station mode

**Simple:** The ESP32 behaves like a normal Wi-Fi client and joins an existing network.

**Technical:** The ESP32 operates as a Wi-Fi station (client) associated with an external access point.
