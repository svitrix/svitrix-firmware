#pragma once

/**
 * @file AppOrderUtils.h
 * @brief Pure merge of a persisted app order with the set of currently-available apps.
 *
 * Extracted for: DisplayManager (loadNativeApps / reorderApps).
 * Used by: DisplayManager to build the live app loop from a single source of
 *          truth (appConfig.appOrder) plus the set of enabled native/weather
 *          apps and loaded custom apps.
 * Tests: test/test_native/test_apporder/
 *
 * The firmware keeps every app (native, weather, custom) in one ordered vector.
 * `appOrder` (persisted as a JSON array of names in NVS) records the desired
 * order. On each rebuild the live set of available apps may differ from the
 * saved order (apps toggled on/off, custom apps added/removed), so the two
 * must be merged deterministically.
 */

#include <vector>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

/**
 * Order the available apps to follow a saved order, appending any extras.
 *
 * Names present in @p savedOrder are emitted first, in saved order, but only
 * if they also appear in @p desired (apps no longer available are dropped).
 * Any name in @p desired that is not in @p savedOrder is appended afterwards,
 * preserving its relative order in @p desired (newly-enabled / new custom apps
 * land at the end). The result contains each name at most once.
 *
 * @param savedOrder Persisted order of app names (may contain stale names).
 * @param desired    Names of all currently-available apps, in default order.
 * @return Ordered, de-duplicated list of the @p desired names.
 */
std::vector<String> orderApps(const std::vector<String>& savedOrder,
                              const std::vector<String>& desired);
