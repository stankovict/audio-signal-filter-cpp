#ifndef SINELOOKUP_H
#define SINELOOKUP_H

#include <map>
#include <string>

// class that stores precomputed sine values
// and allows fast lookup with interpolation
class SineLookup {
private:
    // map: angle in degrees -> sine value
    std::map<int, double> table;

    // true if the table was successfully loaded
    bool loaded;

    // [0, 359]
    int normalizeAngle(int deg) const;

public:
    // constructor
    SineLookup();

    // destructor
    ~SineLookup();

    // load sine values from a text file
    bool loadFromFile(const std::string& filename);

    // get sine value for a given angle
    double getValue(int deg) const;

    // check if table is loaded
    bool isLoaded() const;

    // number of values in the table
    size_t size() const;

    // clear the table and reset state
    void clear();

    // access the internal table
    const std::map<int, double>& getTable() const;
};

#endif