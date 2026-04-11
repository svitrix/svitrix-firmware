---
description: Step-by-step guide to add a new stateless service library to lib/services/
---

Add a new service library to `lib/services/` following the project's architecture conventions.

## Prerequisites

Before starting, the user should specify:
- **Service name** (PascalCase, e.g., `ColorUtils`, `SensorCalc`)
- **Purpose** — what the service does
- **Key methods** — what API it exposes

If not provided, ask the user.

## Steps

1. **Verify the name doesn't conflict:**
   ```bash
   ls lib/services/src/
   ```
   Check that no existing service has the same or similar name.

2. **Create the header file** `lib/services/src/<ServiceName>.h`:
   ```cpp
   #pragma once

   #ifdef UNIT_TEST
   #include <cstdint>
   // Add other standard includes as needed
   #else
   #include <Arduino.h>
   #endif

   // Service API — all functions must be stateless (pure functions)
   // Pass time/state as parameters, don't call millis() or read globals
   ```

   **Rules for the header:**
   - `#pragma once` guard
   - `#ifdef UNIT_TEST` block for native test compatibility (no Arduino dependency)
   - All functions should be stateless — pass time/state as parameters
   - Prefer `const char*` params where possible, `String` for Arduino compat
   - No ArduinoJson dependency — build JSON manually if needed
   - No hardware includes — must compile in `native_test` environment

3. **Create the implementation** `lib/services/src/<ServiceName>.cpp`:
   - Include only the header and standard libs
   - Keep functions pure — deterministic, no side effects
   - Use `std::function` or function pointers for callback injection if needed

4. **Create the test suite** `test/test_native/test_<snake_case>/test_<snake_case>.cpp`:
   ```cpp
   #include <unity.h>
   #include "<ServiceName>.h"

   void setUp() {}
   void tearDown() {}

   void test_<descriptive_name>()
   {
       // Arrange
       // Act
       // Assert
       TEST_ASSERT_...;
   }

   int main()
   {
       UNITY_BEGIN();
       RUN_TEST(test_<descriptive_name>);
       return UNITY_END();
   }
   ```

   **Test rules:**
   - 100% coverage target — test every public function
   - Test edge cases: empty strings, zero values, null pointers, overflow
   - Use Unity framework (`TEST_ASSERT_*` macros)
   - No mocks needed for stateless services (that's the point)

5. **Run tests:**
   ```bash
   pio test -e native_test
   ```
   Verify new tests pass and existing tests are not broken.

6. **Update documentation:**

   **lib/services/CLAUDE.md** — add to the Service Map table:
   ```
   | **<ServiceName>** | <purpose> | Yes | `<method1>()`, `<method2>()` |
   ```
   Also update:
   - Dependency Graph section (if it depends on other services)
   - Who Uses What table (which module will consume it)
   - Test Coverage table
   - Update the service count in the header line

   **Root CLAUDE.md** — update:
   - Service count in Project Structure (`lib/services/` line)
   - Service count in LIBRARIES box
   - Service consumption map (add consumer entry)

7. **Wire it up** in the consuming module:
   - Add `#include "<ServiceName>.h"` in the consumer
   - Call service functions (no injection needed for stateless services)

8. **Verify build:**
   ```bash
   pio run -e ulanzi
   ```
   Check binary size delta — report it.

9. **Report:**
   ```
   === New Service Report ===
   Service: <ServiceName>
   Files created:
   - lib/services/src/<ServiceName>.h
   - lib/services/src/<ServiceName>.cpp
   - test/test_native/test_<snake_case>/test_<snake_case>.cpp

   Tests: X new tests, all passing
   Binary size: +Y bytes (Z.Z%)
   Docs updated: lib/services/CLAUDE.md, CLAUDE.md (root)
   ```

## Design Principles (from lib/services/CLAUDE.md)

1. **Keep stateless** — pass time/state as parameters, don't call `millis()` or read globals
2. **Add tests** in `test/test_native/test_<name>/` — maintain 100% coverage
3. **Use Arduino `String`** for compatibility but prefer `const char*` params where possible
4. **No ArduinoJson** — build JSON manually for native test compatibility
5. **No hardware includes** — services must compile in `native_test` environment
6. **Update the docs** when created
