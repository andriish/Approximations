#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "logapprox.h"

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
            std::cout << "Usage: log_speed_bench [--samples N]\n";
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
        values.push_back(0.5 + fraction);
    }

    double stdSum = 0.0;
    double fastSum = 0.0;
    double maxAbsError = 0.0;

    const auto stdStart = std::chrono::steady_clock::now();
    for (double value : values) {
        stdSum += std::log(value);
    }
    const auto stdEnd = std::chrono::steady_clock::now();

    const auto fastStart = std::chrono::steady_clock::now();
    for (double value : values) {
        const double approx = fastLn(value, &fastLog2p6<double>);
        fastSum += approx;
        const double error = std::abs(std::log(value) - approx);
        maxAbsError = std::max(maxAbsError, error);
    }
    const auto fastEnd = std::chrono::steady_clock::now();

    const auto stdDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(stdEnd - stdStart).count();
    const auto fastDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(fastEnd - fastStart).count();
    const double speedup = static_cast<double>(stdDurationUs) / static_cast<double>(fastDurationUs);

    std::cout << std::setprecision(16);
    std::cout << "samples=" << options.samples << "\n";
    std::cout << "std::log total=" << stdSum << "\n";
    std::cout << "fastLog total=" << fastSum << "\n";
    std::cout << "max_abs_error=" << maxAbsError << "\n";
    std::cout << "std::log time_us=" << stdDurationUs << "\n";
    std::cout << "fastLog time_us=" << fastDurationUs << "\n";
    std::cout << "speedup=" << speedup << "x\n";

    return 0;
}
