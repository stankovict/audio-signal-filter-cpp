#include "BandPassFilter.h"

// constructor MUST explicitly call the virtual base class Filter
BandPassFilter::BandPassFilter(double alpha, int mod_deg, const SineLookup& sine_lookup)
    : Filter(alpha, mod_deg, sine_lookup),
    LowPassFilter(alpha, mod_deg, sine_lookup),
    HighPassFilter(alpha, mod_deg, sine_lookup) 
{
	// constructor can be empty because all initialization is done in base classes
}

// apply band-pass filter by chaining filters
std::vector<double> BandPassFilter::apply(const std::vector<double>& input) {
    if (input.empty()) return {};

    // first apply low-pass using LowPassFilter's implementation
    std::vector<double> temp = LowPassFilter::apply(input);

    // then apply high-pass on the result using HighPassFilter's implementation
    return HighPassFilter::apply(temp);
}