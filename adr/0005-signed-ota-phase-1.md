# ADR 0005: Signed OTA — Phase 1 (structural pre-check)

**Status:** Accepted
**Date:** 2026-05-13

## Context

OTA firmware updates are currently delivered over HTTPS with a pinned root CA ([src/data/cert.h](../src/data/cert.h)). That defends against ordinary MITM on the transport, but not against:

1. **Compromised CA** — historically real (DigiNotar 2011, Symantec 2017). A maliciously issued certificate for the update host bypasses pinning if it's signed by a trusted root.
2. **DNS or ARP hijack** on the client's network — the device may be tricked into talking to an attacker's server that happens to present a valid certificate (e.g., MITM proxy on an enterprise / hotel / cafe WiFi using a root cert pre-installed at provisioning).
3. **TLS implementation bugs** — historical (Heartbleed, etc).
4. **Provider compromise** — if the artifact host is breached, valid-looking firmware can be served.

The right defense is payload-level authenticity: the device verifies a cryptographic signature over the firmware bytes using a public key that lives only in the firmware itself.

Doing this *correctly and atomically* requires intercepting the OTA flow between "firmware bytes written to OTA partition" and "partition marked bootable." Today's flow uses Arduino-ESP32's `httpUpdate.update()`, which performs both steps in one call — there's no public hook to verify before commit. A correct implementation needs custom streaming download + hashing + `mbedtls_pk_verify(...)` against a built-in public key, followed by either `Update.end(true)` (commit) or `Update.abort()` (rollback). That's a non-trivial rewrite of `UpdateManager::updateFirmware()`.

Threat model for Svitrix specifically:
- Personal LED clock on a home network — low attack surface.
- If the firmware is published OSS and others install it on their devices, each of their home networks becomes part of the threat model and the absence of signed OTA matters.

We chose to land signed OTA in two phases rather than blocking the project on a full implementation, so the configuration surface (flag, key slot, `.sig` URL convention) and the test plumbing land first and Phase 2 is purely a verifier swap.

## Decision

Land signed OTA in two phases:

### Phase 1 (this ADR, shipped now)

- New compile-time constant `kOtaUpdatePublicKeyPem` in [src/data/ota_pubkey.h](../src/data/ota_pubkey.h) (default empty).
- New `SystemConfig::verifyUpdateSignature` boolean flag, NVS-persisted (key `VFYUPD`), default false. Also overridable via `/dev.json` key `verify_update_signature`.
- New service [lib/services/src/SignatureVerifier.h](../lib/services/src/SignatureVerifier.h) with:
  - `String buildSignatureUrl(const String& firmwareUrl)` — appends `.sig`
  - `bool isPlausibleRsa2048SignatureBase64(const String& sigBase64)` — structural check (length, character set, padding); does NOT cryptographically verify
- `UpdateManager::updateFirmware()` gating: when the flag is on, before calling `httpUpdate.update()`:
  1. Refuse the update if no public key is compiled in.
  2. Fetch `<firmwareUrl>.sig` over HTTPS with the same pinned CA.
  3. Abort if the response is not HTTP 200 or the body is not plausibly an RSA-2048 base64 signature.
  4. If checks pass, proceed with the original update path.

This catches the obvious failure modes (signature file missing, accidental truncation, HTML error pages) before any partition writes happen, and establishes the configuration surface (flag + key) that Phase 2 will plug into. **It does not yet provide cryptographic protection** — a structurally-valid signature from any source passes the Phase 1 check.

### Phase 2 (out of scope for this ADR)

Cryptographic verification of firmware bytes against `kOtaUpdatePublicKeyPem` is **not** part of this decision. It is forward work. When Phase 2 lands, it will get its own ADR documenting the **decision actually made** at that time (algorithm choice, partition handling, rollback semantics). This ADR records only what shipped: the structural pre-check and the configuration surface.

## Consequences

**Easier:**
- Flag exists from now on — any device can be moved into Phase 1 mode by NVS toggle. No firmware redeployment to acquire the configuration surface.
- Phase 2 is well-scoped: drop in mbedtls calls and partition handling; everything else (flag, URL conventions, service stub, tests) is done.
- Tests catch base64 / URL regressions in CI today via [test/test_native/test_signature_verifier/](../test/test_native/test_signature_verifier/).

**Harder:**
- Phase 1 is honest scaffolding but easy to misread as security. Documentation must be explicit: structural check ≠ cryptographic verification. Setting `verifyUpdateSignature=true` today buys you "we'll refuse updates that lack a `.sig` file" — not "we'll refuse updates that aren't signed by us."
- Slight regression in convenience: enabling the flag now requires the update host to serve a `.sig` next to every `.bin`, even though we don't yet verify the content.

**Must live with:**
- Until Phase 2 ships, an attacker who can serve any 256-byte file at `<firmware>.sig` bypasses the Phase 1 check. The mitigation is to leave the flag off until Phase 2.

## Alternatives considered

- **Skip Phase 1, jump straight to Phase 2.** Lower risk of confusion ("flag exists but doesn't really verify"). Rejected — Phase 2 is a multi-hour rewrite that needs hardware testing; Phase 1 lets us land the configuration surface and tests now, ship the rewrite incrementally.
- **Fake-implement Phase 2 in software now without hardware verification.** Rejected — running cryptographic verification in production code that's never been validated on a real device is worse than not having it. False sense of security plus boot brick risk if the verifier rejects valid firmware.
- **Use ESP-IDF Secure Boot v2 instead of application-level signing.** Burns eFuses, irreversible, kills serial-recovery debug path. Wrong trade-off for a hobby/prosumer device.
- **Use HTTPUpdate's built-in MD5 verification.** MD5 is a checksum, not a signature — defends against transit corruption, not malicious tampering. Already covered by HTTPS integrity.
- **Sign with ECDSA (P-256) instead of RSA-2048.** Smaller signatures (64 vs 256 bytes), faster verify. Reasonable choice for Phase 2. Phase 1 picks RSA-2048 to match the most common existing OpenSSL workflow; revisit at Phase 2.

## References

- [src/data/ota_pubkey.h](../src/data/ota_pubkey.h) — public key embedding point, with key generation/signing recipe in the file header
- [lib/services/src/SignatureVerifier.h](../lib/services/src/SignatureVerifier.h) — Phase 1 structural-check API
- [src/UpdateManager/UpdateManager.cpp](../src/UpdateManager/UpdateManager.cpp) — `prefetchAndCheckSignature()` is the integration point
- [ADR 0003](0003-stateless-services.md) — why the verifier lives in `lib/services/`
- Background on partition rollback: [ESP-IDF over-the-air updates](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ota.html)
