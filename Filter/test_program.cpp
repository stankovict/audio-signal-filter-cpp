#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>
#include <cassert>

#include "../SineLookupLibrary/SineLookup.h"
#include "Filter.h"
#include "LowPassFilter.h"
#include "HighPassFilter.h"
#include "BandPassFilter.h"
#include "WavWriter.h"

 // constants used in tests
constexpr double EPSILON = 0.01;
constexpr double PI = 3.14159265358979323846;

// counters for statistics
int total_tests = 0;
int passed_tests = 0;

// helper function for comparing floating point values, with tolerance
bool approxEqual(double a, double b, double epsilon = EPSILON) {
    return std::abs(a - b) < epsilon;
}

// prints result of a single test
void reportTest(const std::string& testName, bool passed) {
    total_tests++;

    if (passed) {
        passed_tests++;
        std::cout << "  OK  - " << testName << std::endl;
    }
    else {
        std::cout << "  FAIL- " << testName << std::endl;
    }
}

// TEST 1: SINELOOKUP FILE LOADING
void test_SineLookup_Loading() {
    std::cout << "\nTest1 SineLookup file loading" << std::endl;

    // loading a file that does not exist
    {
        SineLookup lookup;
        bool result = lookup.loadFromFile("non_existing_file.txt");
        reportTest("Reject missing file", !result);
    }

    // loading a valid sine table
    {
        SineLookup lookup;
        bool result = lookup.loadFromFile("../../Filter/sine_table.txt");
        reportTest("Load valid table", result && lookup.isLoaded());
    }

    // check that table actually has values
    {
        SineLookup lookup;
        lookup.loadFromFile("../../Filter/sine_table.txt");
        reportTest("Sine table is not empty", lookup.size() > 0);
    }
}

// TEST 2: SINELOOKUP VALUES

void test_SineLookup_ExactValues() {
    std::cout << "\nTest2 SineLookup angle values" << std::endl;

    SineLookup lookup;
    lookup.loadFromFile("../../Filter/sine_table.txt");

    // important angles
    struct TestCase {
        int degree;
        double expected;
        std::string name;
    };

    std::vector<TestCase> cases = {
        {0,   0.0,  "sin(0)"},
        {90,  1.0,  "sin(90)"},
        {180, 0.0,  "sin(180)"},
        {270, -1.0, "sin(270)"},
        {360, 0.0,  "sin(360)"}
    };

    for (const auto& tc : cases) {
        double value = lookup.getValue(tc.degree);
        reportTest(tc.name, approxEqual(value, tc.expected));
    }
}


// TEST 3: SINELOOKUP INTERPOLATION

void test_SineLookup_Interpolation() {
    std::cout << "\nTest3 SineLookup interpolation" << std::endl;

    SineLookup lookup;
    lookup.loadFromFile("../../Filter/sine_table.txt");

    // angles that might not be stored directly in the table
    // sine table file with missing values to be implemented
    std::vector<int> testAngles = { 45, 135, 225, 315 };

    for (int deg : testAngles) {
        double lookupValue = lookup.getValue(deg);
        double realValue = std::sin(deg * PI / 180.0);

        // allow some error because this is interpolation
        bool passed = std::abs(lookupValue - realValue) < 0.1;
        reportTest("Interpolated sin(" + std::to_string(deg) + ")", passed);
    }
}

// TEST 4: SINELOOKUP NORMALIZATION
void test_SineLookup_AngleNormalization() {
    std::cout << "\nTest4 SineLookup normalization" << std::endl;

    SineLookup lookup;
    lookup.loadFromFile("../../Filter/sine_table.txt");

    // angles outside [0, 360] should behave correctly
    struct TestCase {
        int input;
        int normalized;
        std::string name;
    };

    std::vector<TestCase> cases = {
        {370, 10,  "370 -> 10"},
        {720, 0,   "720 -> 0"},
        {-90, 270, "-90 -> 270"},
        {-180,180, "-180 -> 180"},
        {450, 90,  "450 -> 90"}
    };

    for (const auto& tc : cases) {
        double v1 = lookup.getValue(tc.input);
        double v2 = lookup.getValue(tc.normalized);
        reportTest(tc.name, approxEqual(v1, v2));
    }
}


// TEST 5: FILTERS EDGE CASES
void test_Filters_EdgeCases() {
    std::cout << "\nTest5 Filters edge cases" << std::endl;

    SineLookup lookup;
    lookup.loadFromFile("../../Filter/sine_table.txt");

    // empty input
    {
        LowPassFilter filter(0.15, 5, lookup);
        std::vector<double> empty;
        auto result = filter.apply(empty);
        reportTest("Empty input signal", result.empty());
    }

    // single sample
    {
        LowPassFilter filter(0.15, 5, lookup);
        std::vector<double> single = { 0.5 };
        auto result = filter.apply(single);
        reportTest("Single sample input", result.size() == 1);
    }

    // all zeros
    {
        LowPassFilter filter(0.15, 5, lookup);
        std::vector<double> zeros(100, 0.0);
        auto result = filter.apply(zeros);

        bool allZero = true;
        for (double v : result) {
            if (std::abs(v) > EPSILON) {
                allZero = false;
                break;
            }
        }

        reportTest("Zero input produces zero output", allZero);
    }

    // large alternating values
    {
        HighPassFilter filter(0.2, 3, lookup);
        std::vector<double> extreme = { 1.0, -1.0, 1.0, -1.0 };
        auto result = filter.apply(extreme);

        bool stable = true;
        for (double v : result) {
            if (std::abs(v) > 10.0) {
                stable = false;
                break;
            }
        }

        reportTest("Extreme values stay stable", stable);
    }
}

// TEST 6: FILTERS BASIC FUNCTIONALITY
void test_Filters_Functionality() {
    std::cout << "\nTest6 Filters basic" << std::endl;

    SineLookup lookup;
    lookup.loadFromFile("../../Filter/sine_table.txt");

    // constant signal
    std::vector<double> dc(50, 1.0);

    // low-pass passes DC
    {
        LowPassFilter lpf(0.5, 5, lookup);
        auto result = lpf.apply(dc);
        reportTest("Low-pass passes DC", approxEqual(result.back(), 1.0, 0.2));
    }

    // high-pass removes DC
    {
        HighPassFilter hpf(0.5, 5, lookup);
        auto result = hpf.apply(dc);
        reportTest("High-pass removes DC", std::abs(result.back()) < 0.5);
    }

    // band-pass should run without errors
    {
        BandPassFilter bpf(0.3, 5, lookup);
        auto result = bpf.apply(dc);
        reportTest("Band-pass runs correctly", result.size() == dc.size());
    }
}


// TEST 7: FILTER OUTPUT LENGTH
void test_Filters_OutputLength() {
    std::cout << "\nTest7 Filters output" << std::endl;

    SineLookup lookup;
    lookup.loadFromFile("../../Filter/sine_table.txt");

    std::vector<double> signal(100, 0.5);

    reportTest("Low-pass length preserved",
        LowPassFilter(0.15, 5, lookup).apply(signal).size() == signal.size());

    reportTest("High-pass length preserved",
        HighPassFilter(0.2, 3, lookup).apply(signal).size() == signal.size());

    reportTest("Band-pass length preserved",
        BandPassFilter(0.1, 7, lookup).apply(signal).size() == signal.size());
}


// TEST 8: MODULATION EFFECT
void test_Modulation() {
    std::cout << "\nTest8 Modulation" << std::endl;

    SineLookup lookup;
    lookup.loadFromFile("../../Filter/sine_table.txt");

    // constant signal should cause variation
    std::vector<double> signal(360, 1.0);

    LowPassFilter filter(0.5, 1, lookup);
    auto result = filter.apply(signal);

    bool variation = false;
    for (size_t i = 1; i < result.size(); ++i) {
        if (!approxEqual(result[i], result[i - 1], 0.001)) {
            variation = true;
            break;
        }
    }

    reportTest("Modulation affects output", variation);
}


// TEST 9: WAV

void test_WavWriter() {
    std::cout << "\nTest9 WAV writer" << std::endl;

    // empty signal should be rejected
    {
        std::vector<double> empty;
        reportTest("Reject empty WAV",
            !WavWriter::writeWav("test_empty.wav", empty));
    }

    // valid file creation
    {
        std::vector<double> signal = { 0.0, 0.5, 1.0, 0.5, 0.0, -0.5, -1.0 };
        bool ok = WavWriter::writeWav("test_valid.wav", signal);

        std::ifstream f("test_valid.wav", std::ios::binary);
        reportTest("Create valid WAV", ok && f.good());
    }

    // values outside [-1,1] should be normalized
    {
        std::vector<double> extreme = { -10.0, 10.0, 5.0, -5.0 };
        reportTest("Normalize extreme values",
            WavWriter::writeWav("test_clipped.wav", extreme));
    }
}

// TEST 10: FULL INTEGRATION
void test_Integration() {
    std::cout << "\nTest10 Full test" << std::endl;

    SineLookup lookup;
    reportTest("Load sine table",
        lookup.loadFromFile("../../Filter/sine_table.txt"));

    std::vector<double> signal;
    for (int i = 0; i < 100; ++i) {
        double t = i / 100.0;
        signal.push_back(std::sin(2.0 * PI * 5.0 * t));
    }

    LowPassFilter filter(0.15, 5, lookup);
    auto filtered = filter.apply(signal);

    reportTest("Filtering works", filtered.size() == signal.size());
    reportTest("Save WAV file",
        WavWriter::writeWav("test_integration.wav", filtered));
}


// TEST 11: PERFORMANCE
void test_Performance() {
    std::cout << "\nTest11 Performance" << std::endl;

    SineLookup lookup;
    lookup.loadFromFile("../../Filter/sine_table.txt");

    // large signal
    {
        std::vector<double> large(10000, 0.5);
        LowPassFilter filter(0.15, 5, lookup);
        reportTest("Large signal (10k samples)",
            filter.apply(large).size() == large.size());
    }

    // fast modulation
    {
        std::vector<double> signal(1000, 0.5);
        LowPassFilter filter(0.15, 100, lookup);
        reportTest("Fast modulation",
            filter.apply(signal).size() == signal.size());
    }

    // extreme alpha values
    {
        std::vector<double> signal(100, 0.5);
        reportTest("Extreme alpha values",
            LowPassFilter(0.001, 5, lookup).apply(signal).size() == 100 &&
            LowPassFilter(0.999, 5, lookup).apply(signal).size() == 100);
    }
}

int main() {
    std::cout << "Filter Test Program\n" << std::endl;

    std::ifstream check("../../Filter/sine_table.txt");
    if (!check.good()) {
        std::cerr << "Error: sine_table.txt not found." << std::endl;
        return 1;
    }

    test_SineLookup_Loading();
    test_SineLookup_ExactValues();
    test_SineLookup_Interpolation();
    test_SineLookup_AngleNormalization();
    test_Filters_EdgeCases();
    test_Filters_Functionality();
    test_Filters_OutputLength();
    test_Modulation();
    test_WavWriter();
    test_Integration();
    test_Performance();

    std::cout << "\nPASSED: " << passed_tests
        << " / " << total_tests << std::endl;

    return (passed_tests == total_tests) ? 0 : 1;
}