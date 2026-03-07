#include "GammaUtils.h"

// Firmware gamma curve: two-segment log mapping from brightness to gamma exponent.
// Parameters match DisplayManager::gammaCorrection() constants.
float calculateGamma(float brightness)
{
    return logMap(brightness, 2, 180, 0.535, 2.3, 1.9);
}
