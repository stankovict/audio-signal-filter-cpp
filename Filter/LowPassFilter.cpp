#include "LowPassFilter.h"

// constructor just passes stuff to base class
LowPassFilter::LowPassFilter(double alpha, int mod_deg, const SineLookup& sine_lookup)
    : Filter(alpha, mod_deg, sine_lookup) {
}

// apply low-pass filter to a vector of samples
std::vector<double> LowPassFilter::apply(const std::vector<double>& input) {
    if (input.empty()) return {}; // nothing to do

    std::vector<double> out(input.size());

    // first sample is the same as input
    out[0] = input[0];

    // go through the rest of the samples
    for (size_t n = 1; n < input.size(); ++n) {
        int deg = (n * mod_deg) % 360;
        double sine_val = sine_lookup.getValue(deg);
        double m = 1.0 + 0.1 * sine_val;

        out[n] = out[n - 1] + alpha * (m * input[n] - out[n - 1]);
    }

    return out;
}