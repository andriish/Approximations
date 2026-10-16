#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "expexpapprox.h"

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
            std::cout << "Usage: expexp_speed_bench [--samples N]\n";
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
        const double x = -36.0 + 42.0 * fraction;
        values.push_back(x);
    }

    double stdSum = 0.0;
    double fastSum = 0.0;
    double maxRelError = 0.0;

    const auto stdStart = std::chrono::steady_clock::now();
    for (double value : values) {
        const double reference = std::exp(std::exp(value));
        stdSum += reference;
    }
    const auto stdEnd = std::chrono::steady_clock::now();

    const auto fastStart = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < values.size(); ++i) {
        const double approx = expexp(values[i]);
        fastSum += approx;
    }
    const auto fastEnd = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < values.size(); ++i) {
        const long double reference = std::exp(std::exp(static_cast<long double>(values[i])));
        const double approx = expexp(values[i]);
        maxRelError = std::max(maxRelError, static_cast<double>(std::fabs((approx - reference) / reference)));
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
    std::cout << "implementation: expexp\n";
    std::cout << "exp(exp(x)) total=" << stdSum << "\n";
    std::cout << "expexp total=" << fastSum << "\n";
    std::cout << "max_rel_error=" << maxRelError << "\n";
    std::cout << "exp(exp(x)) time_us=" << stdDurationUs << "\n";
    std::cout << "expexp time_us=" << fastDurationUs << "\n";
    std::cout << "exp(exp(x)) calls_per_us=" << stdCallsPerUs << "\n";
    std::cout << "expexp calls_per_us=" << fastCallsPerUs << "\n";
    std::cout << "speedup=" << speedup << "x\n";

    return 0;
}
