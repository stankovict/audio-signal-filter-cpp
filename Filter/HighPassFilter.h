#ifndef HIGHPASSFILTER_H
#define HIGHPASSFILTER_H
#include "Filter.h"

// high-pass filter (lets high frequencies pass)
// Formula: y[n] = alpha * (y[n-1] + m*x[n] - m*x[n-1])
class HighPassFilter : public virtual Filter {  // DODAJ 'virtual'
public:
    // just save alpha, mod_deg, and sine table
    HighPassFilter(double alpha, int mod_deg, const SineLookup& sine_lookup);
    // apply the filter to a vector of samples
    std::vector<double> apply(const std::vector<double>& input) override;
};
#endif