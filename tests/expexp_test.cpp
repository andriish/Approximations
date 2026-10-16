#include <cfloat>
#include <cmath>
#include <cstdio>
#include <limits>

#include "expexpapprox.h"

namespace {

constexpr double kMaxRelError = 1e-12;

long double reference(double x) {
    return std::exp(std::exp(static_cast<long double>(x)));
}

int failures = 0;

void check(bool ok, const char* what, double x) {
    if (!ok) {
        std::printf("FAIL: %s (x=%.17g, got %.17g)\n", what, x, expexp(x));
        ++failures;
    }
}

}  // namespace

int main() {
    const double inf = std::numeric_limits<double>::infinity();
    const double xMaxFinite = std::log(std::log(DBL_MAX));

    double worstError = 0.0;
    double worstX = 0.0;
    for (double x = -40.0; x < xMaxFinite - 1e-6; x += 1e-4) {
        const long double ref = reference(x);
        const double error = static_cast<double>(std::fabs((expexp(x) - ref) / ref));
        if (error > worstError) {
            worstError = error;
            worstX = x;
        }
    }
    std::printf("max relative error %.3e at x=%.6f (limit %.1e)\n", worstError, worstX, kMaxRelError);
    check(worstError <= kMaxRelError, "relative error above limit", worstX);

    check(expexp(-inf) == 1.0, "-inf", -inf);
    check(expexp(-1000.0) == 1.0, "very negative", -1000.0);
    check(std::fabs(expexp(-0.0) / std::exp(1.0) - 1.0) < 1e-14, "negative zero", -0.0);
    check(expexp(inf) == inf, "+inf", inf);
    check(expexp(xMaxFinite + 1e-3) == inf, "overflow", xMaxFinite + 1e-3);
    check(std::isnan(expexp(std::numeric_limits<double>::quiet_NaN())), "NaN", std::numeric_limits<double>::quiet_NaN());

    // Just below the overflow threshold the result must still be finite.
    check(std::isfinite(expexp(xMaxFinite - 1e-6)), "finite below overflow threshold", xMaxFinite - 1e-6);

    return failures == 0 ? 0 : 1;
}
