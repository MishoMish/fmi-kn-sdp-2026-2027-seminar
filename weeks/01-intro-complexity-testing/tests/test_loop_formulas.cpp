#include <catch_amalgamated.hpp>

#include <cstdint>

#include "loop_formulas.h"
#include "loops.h"

// Each test compares the closed formula with the real loop for every n
// in a range. INFO() prints n when a check fails, so you see where.

TEST_CASE("Task 1a: stepped loop", "[task1]") {
    for (std::uint64_t n = 0; n <= 300; ++n) {
        INFO("n = " << n);
        REQUIRE(formulaStepped(n) == loops::stepped(n));
    }
}

TEST_CASE("Task 1b: square loop", "[task1]") {
    for (std::uint64_t n = 0; n <= 300; ++n) {
        INFO("n = " << n);
        REQUIRE(formulaSquare(n) == loops::square(n));
    }
}

TEST_CASE("Task 1c: triangle loop", "[task1]") {
    for (std::uint64_t n = 0; n <= 300; ++n) {
        INFO("n = " << n);
        REQUIRE(formulaTriangle(n) == loops::triangle(n));
    }
}

TEST_CASE("Task 1d: doubling loop", "[task1]") {
    for (std::uint64_t n = 0; n <= 5000; ++n) {
        INFO("n = " << n);
        REQUIRE(formulaDoubling(n) == loops::doubling(n));
    }
    SECTION("large n near powers of two") {
        for (int k = 1; k < 63; ++k) {
            const std::uint64_t p = std::uint64_t{1} << k;
            for (std::uint64_t n : {p - 1, p, p + 1}) {
                INFO("n = " << n);
                REQUIRE(formulaDoubling(n) == loops::doubling(n));
            }
        }
    }
}

TEST_CASE("Task 1e: triple loop", "[task1]") {
    for (std::uint64_t n = 0; n <= 120; ++n) {
        INFO("n = " << n);
        REQUIRE(formulaTriple(n) == loops::triple(n));
    }
}

TEST_CASE("Task 1f: halving sum", "[task1]") {
    for (std::uint64_t n = 0; n <= 3000; ++n) {
        INFO("n = " << n);
        REQUIRE(formulaHalvingSum(n) == loops::halvingSum(n));
    }
}
