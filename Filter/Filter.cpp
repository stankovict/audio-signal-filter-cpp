#include "Filter.h"
#include <cmath>

// some commented code is moved to SineLookup class, but left here for reference

// constructor just saves the stuff
Filter::Filter(double alpha, int mod_deg, const SineLookup& sine_lookup)
    : alpha(alpha), mod_deg(mod_deg), sine_lookup(sine_lookup) {
}

/*
// make sure the angle is always 0..360
int Filter::normalizeAngle(int deg) const {
    deg = deg % 360;
    if (deg < 0) deg += 360;
    return deg;
}
*/

// get sine value for a degree using lookup table
// if exact degree is missing, do simple linear interpolation
/*
double Filter::getSineValue(int deg) const {
    deg = normalizeAngle(deg);

    // check if we have exact value
    auto it = sine_table.find(deg);
    if (it != sine_table.end()) return it->second;

    // find the first value bigger or equal than deg
    auto upper = sine_table.lower_bound(deg);

    // if deg is bigger than all keys, wrap around (like 359 -> 0)
    if (upper == sine_table.end()) {
        auto last = std::prev(sine_table.end());
        auto first = sine_table.begin();

        int deg1 = last->first;
        double val1 = last->second;
        int deg2 = first->first + 360; // wrap
        double val2 = first->second;

        double t = double(deg - deg1) / (deg2 - deg1);
        return val1 + t * (val2 - val1);
    }

    // if deg is smaller than the first key
    if (upper == sine_table.begin()) return upper->second;

    // normal case: interpolate between lower and upper
    auto lower = std::prev(upper);
    int deg1 = lower->first;
    double val1 = lower->second;
    int deg2 = upper->first;
    double val2 = upper->second;

    double t = double(deg - deg1) / (deg2 - deg1);
    return val1 + t * (val2 - val1);
}
*/