#include <catch_amalgamated.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "floating.h"

namespace {
const double kInf = std::numeric_limits<double>::infinity();
const double kNaN = std::numeric_limits<double>::quiet_NaN();
}  // namespace

TEST_CASE("Task 5a: isNaN", "[task5]") {
    REQUIRE(isNaN(kNaN));
    REQUIRE(isNaN(0.0 * kInf));
    REQUIRE(isNaN(kInf - kInf));
    REQUIRE_FALSE(isNaN(0.0));
    REQUIRE_FALSE(isNaN(-0.0));
    REQUIRE_FALSE(isNaN(kInf));
    REQUIRE_FALSE(isNaN(-kInf));
    REQUIRE_FALSE(isNaN(1e308));
}

TEST_CASE("Task 5b: isNegativeZero", "[task5]") {
    REQUIRE(isNegativeZero(-0.0));
    REQUIRE(isNegativeZero(-1.0 * 0.0));
    REQUIRE_FALSE(isNegativeZero(0.0));
    REQUIRE_FALSE(isNegativeZero(-1.0));
    REQUIRE_FALSE(isNegativeZero(-1e-320));  // a tiny (subnormal) negative number
    REQUIRE_FALSE(isNegativeZero(-kInf));
    REQUIRE_FALSE(isNegativeZero(kNaN));
}

TEST_CASE("Task 5c: almostEqual", "[task5]") {
    SECTION("the classic") {
        REQUIRE(0.1 + 0.2 != 0.3);
        REQUIRE(almostEqual(0.1 + 0.2, 0.3));
    }
    SECTION("relative tolerance scales with the magnitude") {
        REQUIRE(almostEqual(1e9, 1e9 + 0.5));
        REQUIRE_FALSE(almostEqual(1.0, 1.0 + 1e-6));
    }
    SECTION("absolute tolerance handles values near zero") {
        REQUIRE(almostEqual(1e-20, 0.0));
        REQUIRE_FALSE(almostEqual(1e-6, 0.0));
    }
    SECTION("special values") {
        REQUIRE(almostEqual(0.0, -0.0));
        REQUIRE(almostEqual(kInf, kInf));
        REQUIRE_FALSE(almostEqual(kInf, -kInf));
        REQUIRE_FALSE(almostEqual(kInf, 1e308));
        REQUIRE_FALSE(almostEqual(kNaN, kNaN));
        REQUIRE_FALSE(almostEqual(kNaN, 1.0));
    }
}

TEST_CASE("Task 5d: kahanSum beats naive summation", "[task5]") {
    const std::vector<double> tenths(1'000'000, 0.1);

    double naive = 0.0;
    for (double v : tenths) {
        naive += v;
    }
    const double exact = 100000.0;
    const double kahan = kahanSum(tenths);

    INFO("naive = " << naive << ", kahan = " << kahan);
    REQUIRE(std::fabs(kahan - exact) < 1e-8);
    REQUIRE(std::fabs(kahan - exact) < std::fabs(naive - exact));

    SECTION("simple cases still work") {
        REQUIRE(kahanSum({}) == 0.0);
        REQUIRE(kahanSum({1.5}) == 1.5);
        REQUIRE(kahanSum({1.0, 2.0, 3.0}) == 6.0);
    }
}

TEST_CASE("Task 6: nanLastLess is a strict weak ordering with NaN last", "[task6]") {
    SECTION("pairwise behaviour") {
        REQUIRE(nanLastLess(1.0, 2.0));
        REQUIRE_FALSE(nanLastLess(2.0, 1.0));
        REQUIRE_FALSE(nanLastLess(1.0, 1.0));      // irreflexive
        REQUIRE(nanLastLess(1.0, kNaN));           // numbers before NaN
        REQUIRE(nanLastLess(kInf, kNaN));
        REQUIRE_FALSE(nanLastLess(kNaN, 1.0));
        REQUIRE_FALSE(nanLastLess(kNaN, kNaN));    // NaNs are equivalent
    }
    SECTION("std::sort puts NaNs at the end") {
        std::vector<double> v{3.0, kNaN, 1.0, kNaN, 2.0, -kInf, kInf, -0.5, kNaN, 0.0};
        std::sort(v.begin(), v.end(), nanLastLess);
        const auto firstNaN = std::find_if(v.begin(), v.end(),
                                           [](double x) { return std::isnan(x); });
        REQUIRE(firstNaN - v.begin() == 7);
        REQUIRE(std::all_of(firstNaN, v.end(), [](double x) { return std::isnan(x); }));
        REQUIRE(std::is_sorted(v.begin(), firstNaN));
        REQUIRE(v.front() == -kInf);
        REQUIRE(v[6] == kInf);
    }
}
