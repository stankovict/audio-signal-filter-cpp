#ifndef BANDPASSFILTER_H
#define BANDPASSFILTER_H
#include "LowPassFilter.h"
#include "HighPassFilter.h"

// band-pass filter (lets frequencies in a certain range pass)
// done by combining LowPassFilter and HighPassFilter through multiple inheritance
class BandPassFilter : public LowPassFilter, public HighPassFilter { 
public:
    // constructor 
    BandPassFilter(double alpha, int mod_deg, const SineLookup& sine_lookup);
    // apply band-pass filter by chaining low-pass and high-pass
    std::vector<double> apply(const std::vector<double>& input) override;
};
#endif