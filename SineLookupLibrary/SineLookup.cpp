#include "SineLookup.h"
#include <fstream>
#include <sstream>
#include <iostream>

// constructor, lookup table is empty at start
SineLookup::SineLookup() : loaded(false) {}

// destructor
SineLookup::~SineLookup() {
    clear();
}

// second version
int SineLookup::normalizeAngle(int deg) const {
    deg = deg % 360;
    if (deg < 0)
        deg += 360;
    return deg;
}

//load sine values from a text file
bool SineLookup::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "SineLookup: Cannot open file "
            << filename << std::endl;
        return false;
    }

    table.clear();
    loaded = false;

    std::string line;
    int lineNumber = 0;

    // read file line by line
    while (std::getline(file, line)) {
        lineNumber++;

        std::istringstream iss(line);
        int deg;
        double value;

        // integer degree + double value
        if (!(iss >> deg >> value)) {
            std::cerr << "SineLookup: Parse error at line "
                << lineNumber << ": " << line << std::endl;
            table.clear();
            return false;
        }

		// store in map/table
        table[deg] = value;
    }

	// check if table is empty
    if (table.empty()) {
        std::cerr << "SineLookup: Table is empty after loading"
            << std::endl;
        return false;
    }

    loaded = true;
    return true;
}


// get sine value for a given angle
// need to make a test file for interpolation with missing values, currently all values are present

double SineLookup::getValue(int deg) const {
    if (!loaded) {
        std::cerr << "SineLookup: Table not loaded"
            << std::endl;
        return 0.0;
    }
	//normalize to [0, 359]
    deg = normalizeAngle(deg);

	// check if value exists directly
    auto it = table.find(deg);
    if (it != table.end()) {
        return it->second;
    }

	// find the first value greater than deg, interpolation will be done between
    auto upper = table.lower_bound(deg);

	// if deg is greater than all keys, wrap around
    if (upper == table.end()) {
        auto last = std::prev(table.end());
        auto first = table.begin();

		// interpolation between last and first (wrapped)
        int deg1 = last->first;
        double val1 = last->second;

        int deg2 = first->first + 360;
        double val2 = first->second;

        double t = static_cast<double>(deg - deg1)
            / (deg2 - deg1);

        return val1 + t * (val2 - val1);
    }

	// if deg is smaller than the first key
    if (upper == table.begin()) {
        return upper->second;
    }

	// standard case: interpolate between lower and upper
    auto lower = std::prev(upper);

    int deg1 = lower->first;
    double val1 = lower->second;

    int deg2 = upper->first;
    double val2 = upper->second;

    double t = static_cast<double>(deg - deg1)
        / (deg2 - deg1);

    return val1 + t * (val2 - val1);
}


//check loaded state
bool SineLookup::isLoaded() const {
    return loaded;
}

//number of values in the table
size_t SineLookup::size() const {
    return table.size();
}
//reset state
void SineLookup::clear() {
    table.clear();
    loaded = false;
}

//read-only access to internal table
const std::map<int, double>& SineLookup::getTable() const {
    return table;
}