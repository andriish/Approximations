#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "loglogapprox.h"

struct Options {
    std::size_t samples = 1'000'000;
};

Options parseOptions(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--samples" && i + 1 < argc) {
            options.samples = static_cast<std::size_t>(std::stoul(argv[++i]));
        } else if (arg == "--help") {
            std::cout << "Usage: loglog_speed_bench [--samples N]\n";
            std::exit(0);
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            std::exit(1);
        }
    }
    return options;
}

int main(int argc, char** argv) {
    const Options options = parseOptions(argc, argv);
    if (options.samples == 0) {
        std::cerr << "Sample count must be greater than zero.\n";
        return 1;
    }

    std::vector<double> values;
    values.reserve(options.samples);
    for (std::size_t i = 0; i < options.samples; ++i) {
        const double fraction = static_cast<double>(i) / static_cast<double>(std::max<std::size_t>(1, options.samples - 1));
        const double x = std::exp2(2.0 + 20.0 * fraction);
        values.push_back(x);
    }

    double stdSum = 0.0;
    double fastSum = 0.0;
    double maxAbsError = 0.0;

    const auto stdStart = std::chrono::steady_clock::now();
    for (double value : values) {
        const double reference = std::log(std::log(value));
        stdSum += reference;
    }
    const auto stdEnd = std::chrono::steady_clock::now();

    const auto fastStart = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < values.size(); ++i) {
        const double approx = fastLog2Log2(values[i]);
        fastSum += approx;
    }
    const auto fastEnd = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < values.size(); ++i) {
        const double reference = std::log(std::log(values[i]));
        const double approx = fastLog2Log2(values[i]);
        maxAbsError = std::max(maxAbsError, std::abs(reference - approx));
    }

    const auto stdDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(stdEnd - stdStart).count();
    const auto fastDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(fastEnd - fastStart).count();
    const double stdCallsPerUs = stdDurationUs > 0
        ? static_cast<double>(values.size()) / static_cast<double>(stdDurationUs)
        : 0.0;
    const double fastCallsPerUs = fastDurationUs > 0
        ? static_cast<double>(values.size()) / static_cast<double>(fastDurationUs)
        : 0.0;
    const double speedup = fastDurationUs > 0
        ? static_cast<double>(stdDurationUs) / static_cast<double>(fastDurationUs)
        : 0.0;

    std::cout << std::setprecision(17);
    std::cout << "samples=" << options.samples << "\n";
    std::cout << "implementation: C++ loglog\n";
    std::cout << "log(log(x)) total=" << stdSum << "\n";
    std::cout << "C++ fast loglog total=" << fastSum << "\n";
    std::cout << "max_abs_error=" << maxAbsError << "\n";
    std::cout << "log(log(x)) time_us=" << stdDurationUs << "\n";
    std::cout << "C++ fast loglog time_us=" << fastDurationUs << "\n";
    std::cout << "log(log(x)) calls_per_us=" << stdCallsPerUs << "\n";
    std::cout << "C++ fast loglog calls_per_us=" << fastCallsPerUs << "\n";
    std::cout << "speedup=" << speedup << "x\n";

    return 0;
}
