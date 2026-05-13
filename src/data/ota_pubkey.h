#pragma once

/**
 * @file ota_pubkey.h
 * @brief RSA public key for OTA firmware signature verification (Phase 1 stub).
 *
 * Empty by default. To enable signed OTA:
 *   1. Generate a keypair (one-time):
 *        openssl genrsa -out ota_private.pem 2048
 *        openssl rsa -in ota_private.pem -pubout -out ota_public.pem
 *   2. Paste the contents of ota_public.pem into the string literal below.
 *   3. Set `systemConfig.verifyUpdateSignature = true` (via settings UI or NVS).
 *   4. Sign each released `firmware.bin` with the private key:
 *        openssl dgst -sha256 -sign ota_private.pem -out firmware.bin.sig firmware.bin
 *        # then base64-encode for serving over plain HTTPS:
 *        openssl base64 -in firmware.bin.sig -out firmware.bin.sig.b64
 *   5. Publish both `firmware.bin` and `firmware.bin.sig` (base64) at the same URL prefix.
 *
 * Phase 1 (current) verifies that a non-empty, well-formed signature file
 * accompanies every signed OTA download. Actual cryptographic verification
 * (RSA-PSS over SHA-256 of firmware bytes) is Phase 2 — see ADR 0005.
 *
 * SECURITY: keep the private key off the device, off CI logs, and out of the
 * repository. CI step that signs releases is the only place it should appear,
 * via GitHub Actions Secrets.
 */

/// PEM-encoded RSA public key (multi-line literal). Empty means: signature
/// verification cannot be enabled. Activating the flag with an empty key
/// causes UpdateManager to refuse the update.
constexpr const char *kOtaUpdatePublicKeyPem = "";
