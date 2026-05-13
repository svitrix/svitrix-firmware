#pragma once

/**
 * @file SignatureVerifier.h
 * @brief Format validation for base64-encoded OTA firmware signatures.
 *
 * Phase 1 scope: validate that a downloaded `.sig` file is *plausibly* a
 * signature — non-empty, decodes from base64 to the expected byte length
 * range for RSA-2048-SHA256 signatures (256 bytes). This catches the
 * obvious failure modes (404 → empty body, HTML error page, accidental
 * truncation) before we attempt anything heavier.
 *
 * Phase 2 (see ADR 0005): cryptographic verification of the signature
 * against firmware bytes using mbedtls RSA-PSS over SHA-256. Phase 2
 * lives in the firmware-only path because mbedtls is not available in
 * the native test environment; the API surface defined here will gain
 * a `verifySignature(...)` function gated by `#ifndef UNIT_TEST`.
 *
 * Tests: test/test_native/test_signature_verifier/
 */

#include <cstdint>
#include <cstddef>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

/// Expected raw signature size for RSA-2048 = 256 bytes.
constexpr size_t kRsa2048SignatureBytes = 256;

/**
 * Quick sanity check on a base64-encoded signature blob.
 *
 * Returns true iff:
 *   - the string is non-empty after trimming whitespace,
 *   - every character is a valid base64 alphabet character or '=' padding,
 *   - the decoded byte length falls within [kRsa2048SignatureBytes - 2,
 *     kRsa2048SignatureBytes + 2] (small slack to tolerate trailing
 *     newline / mis-padded export tools).
 *
 * Does NOT perform cryptographic verification — that is Phase 2.
 *
 * @param sigBase64 The signature payload exactly as fetched from the
 *                  signature URL (may contain trailing newlines).
 * @return true if the blob is plausibly an RSA-2048 signature.
 */
bool isPlausibleRsa2048SignatureBase64(const String& sigBase64);

/**
 * Construct the conventional signature URL for a firmware URL.
 * Adds the suffix `.sig` to the firmware URL — same scheme used by
 * `openssl dgst -sign`-style workflows and the GitHub release CI step
 * planned for Phase 2.
 *
 * @param firmwareUrl URL of the firmware .bin file. Empty input returns empty.
 * @return The signature URL, or empty string if input is empty.
 */
String buildSignatureUrl(const String& firmwareUrl);
