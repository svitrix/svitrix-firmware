# Security policy

## Supported versions

Svitrix follows [SemVer](https://semver.org/) with pre-release tags (see [.claude/git-workflow.md](.claude/git-workflow.md)). Security fixes are backported to the most recent stable minor release; older minor versions are not patched.

| Version | Supported |
|---------|-----------|
| latest stable `v0.x.0` | ✅ Yes |
| previous stable | ⚠️ Critical fixes only |
| pre-1.0 betas / RCs | ❌ Use the corresponding stable instead |

Run `git tag --sort=-v:refname | head -5` (or check [Releases](https://github.com/svitrix/svitrix-firmware/releases)) to see what's current.

## Reporting a vulnerability

**Please do not open public GitHub issues for vulnerabilities.** A public issue tips off attackers before users can update.

Use one of these private channels:

1. **Preferred — GitHub Private Vulnerability Reporting:**
   <https://github.com/svitrix/svitrix-firmware/security/advisories/new>
   This creates a private advisory only visible to maintainers; we coordinate the fix and credit you in the published advisory.

2. **Fallback — email** the maintainer at `oleksandr.slukovskyi@gmail.com` with the subject prefix `[svitrix-security]`. PGP key not yet published; if you need encrypted communication, mention it and we'll set one up.

Include in your report:
- Affected version(s) — exact tag if possible, otherwise commit hash
- Reproduction steps or proof-of-concept
- Impact and threat model (who can exploit, what they gain)
- Suggested fix if you have one (optional)
- Whether you intend to disclose publicly and on what timeline

## What we commit to

- **Acknowledge** receipt within **3 working days**.
- **Initial assessment** (severity, scope) within **7 working days**.
- **Patch + coordinated disclosure** for confirmed vulnerabilities — typically a stable release within **30 days** for high-severity, longer for complex changes that require breaking-change handling. We will keep you updated.
- **Credit** in the GitHub Security Advisory and the relevant [CHANGELOG.md](CHANGELOG.md) entry, unless you request anonymity.

## Scope

In-scope for this policy:
- Firmware code in `src/`, `lib/`, `tools/`, `web/`
- Build / release tooling under `.github/workflows/`
- Default configuration that ships with the firmware

Out of scope:
- Vulnerabilities in third-party dependencies — please report them upstream first, then notify us if a patch we ship is affected
- User-supplied custom apps or dev.json configurations
- Issues that require physical access to the device and direct access to its GPIO/USB-serial console (the threat model is that of a network-connected appliance, not a tamper-proof secure element)
- ESP32 hardware silicon bugs (report to Espressif)

## Known sharp edges

These are documented limitations, not vulnerabilities, but listed here for transparency:

- **OTA signature verification is partial** — `systemConfig.verifyUpdateSignature` (Phase 1) performs a structural check on a fetched signature file. Cryptographic verification against firmware bytes is Phase 2 work tracked in [ADR 0005](adr/0005-signed-ota-phase-1.md). Do not rely on Phase 1 for authenticity guarantees.
- **No Secure Boot / Flash Encryption** — eFuses are not burned. Physical access allows full firmware extraction and replacement.
- **MQTT credentials in NVS** — stored in plain text. NVS is not encrypted.

## Hall of fame

Reporters of confirmed vulnerabilities are listed here (or via the GitHub Security Advisory) once a fix is released. List will grow as advisories are published.

— *(empty — be the first!)*
