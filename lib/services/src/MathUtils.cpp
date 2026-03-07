#include "MathUtils.h"

// Multiply-then-divide approach avoids floating-point drift for small `places`.
double roundToDecimalPlaces(double value, int places)
{
    double factor = pow(10.0, places);
    return round(value * factor) / factor;
}

// Two-segment logarithmic curve:
//   Lower half  [in_min .. midpoint]  → [out_min .. mid_point_out]
//   Upper half  [midpoint .. in_max]  → [mid_point_out .. out_max]
// Each segment uses  scale * ln(x - offset + 1) + base.
float logMap(float x, float in_min, float in_max, float out_min, float out_max, float mid_point_out)
{
    // Guard: degenerate input range → log(1)=0 would cause division by zero.
    if (in_max <= in_min)
        return out_min;
    if (x < in_min)
        return out_min;
    if (x > in_max)
        return out_max;

    float midpoint = (in_max + in_min) / 2.0;

    // Lower segment: logarithmic rise from out_min to mid_point_out
    float scale = (mid_point_out - out_min) / log(in_max - in_min + 1);
    if (x <= midpoint)
    {
        return scale * log(x - in_min + 1) + out_min;
    }

    // Upper segment: logarithmic rise from mid_point_out to out_max
    float upper_scale = (out_max - mid_point_out) / log(in_max - midpoint + 1);
    return upper_scale * log(x - midpoint + 1) + mid_point_out;
}
