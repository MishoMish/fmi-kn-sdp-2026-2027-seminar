#include <catch_amalgamated.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "binary_search.h"
#include "fixed_array.h"

TEST_CASE("Task 4: lowerBound and upperBound on small inputs", "[task4]") {
    const std::vector<int> v{1, 3, 3, 3, 7, 9};
    const auto b = v.begin();
    REQUIRE(lowerBound(v.begin(), v.end(), 3) - b == 1);
    REQUIRE(upperBound(v.begin(), v.end(), 3) - b == 4);
    REQUIRE(lowerBound(v.begin(), v.end(), 0) - b == 0);   // smaller than everything
    REQUIRE(upperBound(v.begin(), v.end(), 0) - b == 0);
    REQUIRE(lowerBound(v.begin(), v.end(), 10) - b == 6);  // larger than everything
    REQUIRE(lowerBound(v.begin(), v.end(), 5) - b == 4);   // missing, in the middle
    REQUIRE(upperBound(v.begin(), v.end(), 9) - b == 6);
}

TEST_CASE("Task 4: empty and one-element ranges", "[task4]") {
    const std::vector<int> empty;
    REQUIRE(lowerBound(empty.begin(), empty.end(), 5) == empty.end());
    REQUIRE(upperBound(empty.begin(), empty.end(), 5) == empty.end());
    REQUIRE_FALSE(contains(empty.begin(), empty.end(), 5));

    const std::vector<int> one{5};
    REQUIRE(lowerBound(one.begin(), one.end(), 5) == one.begin());
    REQUIRE(upperBound(one.begin(), one.end(), 5) == one.end());
    REQUIRE(contains(one.begin(), one.end(), 5));
    REQUIRE_FALSE(contains(one.begin(), one.end(), 4));
    REQUIRE_FALSE(contains(one.begin(), one.end(), 6));
}

TEST_CASE("Task 4: agrees with std::lower_bound / std::upper_bound on random data", "[task4]") {
    std::mt19937 rng(2026);
    for (int round = 0; round < 500; ++round) {
        std::vector<int> v(rng() % 50);
        for (int& x : v) {
            x = static_cast<int>(rng() % 20);
        }
        std::sort(v.begin(), v.end());
        for (int value = -1; value <= 21; ++value) {
            INFO("round " << round << ", size " << v.size() << ", value " << value);
            REQUIRE(lowerBound(v.begin(), v.end(), value) == std::lower_bound(v.begin(), v.end(), value));
            REQUIRE(upperBound(v.begin(), v.end(), value) == std::upper_bound(v.begin(), v.end(), value));
            REQUIRE(contains(v.begin(), v.end(), value) == std::binary_search(v.begin(), v.end(), value));
        }
    }
}

TEST_CASE("Task 4: custom order, C arrays, FixedArray, strings", "[task4]") {
    SECTION("descending order with std::greater") {
        const std::vector<int> v{9, 7, 7, 4, 1};
        REQUIRE(lowerBound(v.begin(), v.end(), 7, std::greater<>{}) - v.begin() == 1);
        REQUIRE(upperBound(v.begin(), v.end(), 7, std::greater<>{}) - v.begin() == 3);
        REQUIRE(contains(v.begin(), v.end(), 4, std::greater<>{}));
    }
    SECTION("a plain C array") {
        const int a[] = {2, 4, 6, 8};
        REQUIRE(lowerBound(std::begin(a), std::end(a), 6) == a + 2);
    }
    SECTION("FixedArray from Task 1") {
        const FixedArray<int> f(10, {10, 20, 30});
        REQUIRE(contains(f.begin(), f.end(), 20));
        REQUIRE_FALSE(contains(f.begin(), f.end(), 25));
    }
    SECTION("strings") {
        const std::vector<std::string> words{"array", "heap", "list", "tree"};
        REQUIRE(lowerBound(words.begin(), words.end(), std::string("graph")) - words.begin() == 1);
        REQUIRE(contains(words.begin(), words.end(), std::string("list")));
    }
}

TEST_CASE("Task 5: firstTrue on simple predicates", "[task5]") {
    REQUIRE(firstTrue(0, 100, [](std::uint64_t x) { return x >= 37; }) == 37);
    REQUIRE(firstTrue(0, 100, [](std::uint64_t) { return true; }) == 0);
    REQUIRE(firstTrue(0, 100, [](std::uint64_t) { return false; }) == 100);
    REQUIRE(firstTrue(5, 5, [](std::uint64_t) { return true; }) == 5);   // empty range
    REQUIRE(firstTrue(10, 20, [](std::uint64_t x) { return x * x > 200; }) == 15);
}

TEST_CASE("Task 5: firstTrue does not overflow near the top of uint64", "[task5]") {
    const std::uint64_t max = std::numeric_limits<std::uint64_t>::max();
    const std::uint64_t target = max - 3;
    REQUIRE(firstTrue(max - 100, max, [&](std::uint64_t x) { return x >= target; }) == target);
    REQUIRE(firstTrue(0, max, [&](std::uint64_t x) { return x >= target; }) == target);
}

TEST_CASE("Task 5: integerSqrt", "[task5]") {
    REQUIRE(integerSqrt(0) == 0);
    REQUIRE(integerSqrt(1) == 1);
    REQUIRE(integerSqrt(2) == 1);
    REQUIRE(integerSqrt(3) == 1);
    REQUIRE(integerSqrt(4) == 2);
    REQUIRE(integerSqrt(99) == 9);
    REQUIRE(integerSqrt(100) == 10);
    for (std::uint64_t r = 1; r < 3000; ++r) {
        INFO("r = " << r);
        REQUIRE(integerSqrt(r * r) == r);
        REQUIRE(integerSqrt(r * r - 1) == r - 1);
    }
    SECTION("largest inputs") {
        const std::uint64_t max = std::numeric_limits<std::uint64_t>::max();
        REQUIRE(integerSqrt(max) == 4294967295ULL);
        REQUIRE(integerSqrt(4294967295ULL * 4294967295ULL) == 4294967295ULL);
        REQUIRE(integerSqrt(4294967295ULL * 4294967295ULL - 1) == 4294967294ULL);
    }
}
