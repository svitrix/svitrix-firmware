---
description: Full local CI pipeline — tests, build, flash size check
---

Run the complete local CI pipeline for Svitrix firmware. Execute each step sequentially, stop on first failure.

## Steps

1. **Run all unit tests:**
   ```bash
   pio test -e native_test
   ```
   Report: total suites, total tests, pass/fail count.
   If any test fails — STOP and report which suite failed.

2. **Build firmware:**
   ```bash
   pio run -e ulanzi
   ```
   This also triggers the web SPA pre-build via `tools/build_web.py`.
   If build fails — STOP and report the error.

3. **Check flash size:**
   After successful build, check `.pio/build/ulanzi/firmware.bin` size.
   Read the build output for RAM/Flash usage summary.

   Thresholds (firmware partition is ~3.8MB):
   - **< 90%**: OK
   - **90-95%**: WARNING — flash space getting tight
   - **> 95%**: CRITICAL — must optimize before adding features

4. **Summary report:**
   ```
   Tests:     24 suites, XXX tests passed
   Build:     OK
   Firmware:  XXX KB / 3932 KB (XX%)
   Status:    OK | WARNING | CRITICAL
   ```

## Done when

- [ ] All test suites pass (zero failures)
- [ ] Firmware builds without errors
- [ ] Flash usage reported with threshold status
- [ ] Summary table printed
