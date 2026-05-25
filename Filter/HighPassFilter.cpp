#include "HighPassFilter.h"

// constructor just passes stuff to base class
HighPassFilter::HighPassFilter(double alpha, int mod_deg, const SineLookup& sine_lookup)
    : Filter(alpha, mod_deg, sine_lookup) {
}

// apply high-pass filter to a vector of samples
std::vector<double> HighPassFilter::apply(const std::vector<double>& input) {
    // if input is empty, return an empty vector
    if (input.empty()) return {};

    std::vector<double> output(input.size());

    // first output sample is set to 0
    output[0] = 0.0;

    // y[n] = alpha * ( y[n-1] + m * x[n] - m * x[n-1] )
    // where m is a modulation factor based on a sine lookup table
    for (size_t n = 1; n < input.size(); ++n) {
        int deg = (n * mod_deg) % 360;
        double sine_val = sine_lookup.getValue(deg);
        double m = 1.0 + 0.1 * sine_val;
        output[n] = alpha * (output[n - 1] + m * input[n] - m * input[n - 1]);
    }

    return output;
}