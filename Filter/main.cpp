#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

#include "../SineLookupLibrary/SineLookup.h"
#include "Filter.h"
#include "LowPassFilter.h"
#include "HighPassFilter.h"
#include "BandPassFilter.h"
#include "WavWriter.h"

// file name constants
constexpr const char* SINE_TABLE_FILE = "../../Filter/sine_table.txt";
constexpr const char* SIGNAL_FILE = "../../Filter/signal.txt";
constexpr const char* OUTPUT_FILE = "../../Filter/output.wav";

/*
 * loads input signal from a text file, one number per line
 */
bool loadSignal(const std::string& filename, std::vector<double>& signal) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        std::istringstream iss(line);
        double value;

        // try to parse a double from the line
        if (!(iss >> value)) {
            std::cerr << "Error parsing line " << lineNumber
                << ": " << line << std::endl;
            return false;
        }

        signal.push_back(value);
    }

    // signal must not be empty!
    if (signal.empty()) {
        std::cerr << "Error: Input signal is empty" << std::endl;
        return false;
    }

    std::cout << "Loaded " << signal.size()
        << " signal samples." << std::endl;

    return true;
}

/*
 * creates the correct filter based on the given type from command line
 */
std::unique_ptr<Filter> createFilter(
    const std::string& type,
    double alpha,
    int mod_deg,
    const SineLookup& sine_lookup
) {
    if (type == "lowpass") {
        return std::make_unique<LowPassFilter>(alpha, mod_deg, sine_lookup);
    }
    else if (type == "highpass") {
        return std::make_unique<HighPassFilter>(alpha, mod_deg, sine_lookup);
    }
    else if (type == "bandpass") {
        return std::make_unique<BandPassFilter>(alpha, mod_deg, sine_lookup);
    }

    // unknown filter type
    return nullptr;
}

/*
 * information about how to use the program if !=3 arguments are given
 */
void printUsage(const char* programName) {
    std::cout << "HOW TO CORRECTLY USE THE PROGRAM: " << programName
        << " <type> <alpha> <mod_deg>" << std::endl;

    std::cout << "  type    : lowpass | highpass | bandpass" << std::endl;
    std::cout << "  alpha   : filter reaction speed (e.g. 0.15)" << std::endl;
    std::cout << "  mod_deg : modulation speed in degrees (e.g. 5)" << std::endl;
    std::cout << std::endl;

    std::cout << "TRY: " << programName
        << " lowpass 0.15 5" << std::endl;
}

int main(int argc, char* argv[]) {

    // check command-line arguments
	if (argc != 4) { // checks for 3 arguments + program name
        std::cerr << "Error: Invalid number of arguments" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    std::string filterType = argv[1];
    double alpha;
    int mod_deg;

    // parse alpha value
    try {
        alpha = std::stod(argv[2]);
    }
    catch (...) {
        std::cerr << "Error: Invalid alpha value: "
            << argv[2] << std::endl;
        return 1;
    }

    // parse modulation degree
    try {
        mod_deg = std::stoi(argv[3]);
    }
    catch (...) {
        std::cerr << "Error: Invalid mod_deg value: "
            << argv[3] << std::endl;
        return 1;
    }

    // check filter type
    if (filterType != "lowpass" &&
        filterType != "highpass" &&
        filterType != "bandpass") {

        std::cerr << "Error: Unknown filter type: "
            << filterType << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    // info
    std::cout << "Audio Filter" << std::endl;
    std::cout << "Type: " << filterType << std::endl;
    std::cout << "Alpha value: " << alpha << std::endl;
    std::cout << "Modulation degree: " << mod_deg << std::endl;
    std::cout << std::endl;

    // load sine lookup table
    SineLookup sine_lookup;
    std::cout << "Loading sine lookup table from file..." << std::endl;

    if (!sine_lookup.loadFromFile(SINE_TABLE_FILE)) {
        std::cerr << "Error: Failed to load sine table" << std::endl;
        return 1;
    }

    std::cout << "Loaded " << sine_lookup.size()
        << " sine values." << std::endl << std::endl;

    // load signal from file
    std::vector<double> input_signal;
    if (!loadSignal(SIGNAL_FILE, input_signal)) {
        return 1;
    }

	// create filter selected by the user
    std::unique_ptr<Filter> filter =
        createFilter(filterType, alpha, mod_deg, sine_lookup);

    if (!filter) {
        std::cerr << "Error: Failed to create filter" << std::endl;
        return 1;
    }

    // apply filter to input
    std::cout << "\nApplying filter..." << std::endl;
    std::vector<double> output_signal = filter->apply(input_signal);

    if (output_signal.empty()) {
        std::cerr << "Error: Filtering failed" << std::endl;
        return 1;
    }

    // save result as WAV
    std::cout << "Saving output to " << OUTPUT_FILE << "..." << std::endl;
    if (!WavWriter::writeWav(OUTPUT_FILE, output_signal)) {
        std::cerr << "Error: Failed to write WAV file" << std::endl;
        return 1;
    }

    std::cout << "\nFinished successfully." << std::endl;
    std::cout << "Output file: " << OUTPUT_FILE << std::endl;

    return 0;
}