#ifndef LOWPASSFILTER_H
#define LOWPASSFILTER_H
#include "Filter.h"

// low-pass filter (lets low frequencies pass)
// formula: y[n] = y[n-1] + alpha * (m * x[n] - y[n-1])
class LowPassFilter : public virtual Filter {
public:
    // constructor just saves alpha, mod_deg, and sine table
    LowPassFilter(double alpha, int mod_deg, const SineLookup& sine_lookup);
    // apply low-pass filter to a vector of samples
    std::vector<double> apply(const std::vector<double>& input) override;
};
#endif