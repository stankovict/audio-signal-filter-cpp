#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <map>
#include <string>
#include "../SineLookupLibrary/SineLookup.h"

// some commented code is moved to SineLookup class, but left here for reference

/*
 * base class for filters
 * just holds common stuff and functions for all filters
 */
class Filter {
protected:
    double alpha;  // how fast filter reacts
    int mod_deg;   // how fast the modulation sine moves
    //const std::map<int, double>& sine_table; // reference to sine lookup table
    const SineLookup& sine_lookup;

    // get sine value from table for a given degree
    // if exact degree is not in table, do simple linear interpolation
    //double getSineValue(int deg) const;

    // make sure angle is always 0..360
    //int normalizeAngle(int deg) const;

public:
    // constructor just saves alpha, mod_deg, and the table
    Filter(double alpha, int mod_deg, const SineLookup& sine_lookup);

    // destructor
    virtual ~Filter() = default;

    // each filter must have this function to apply filtering
    virtual std::vector<double> apply(const std::vector<double>& input) = 0;
};

#endif