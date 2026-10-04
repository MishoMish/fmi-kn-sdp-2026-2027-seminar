#include <catch_amalgamated.hpp>

#include <random>
#include <string>
#include <vector>

#include "duplicates.h"

namespace {

// Runs all three implementations and checks they agree with `expected`.
template <typename T>
void checkAll(const std::vector<T>& values, bool expected) {
    CHECK(hasDuplicateNaive(values) == expected);
    CHECK(hasDuplicateSorting(values) == expected);
    CHECK(hasDuplicateHashing(values) == expected);
}

}  // namespace

TEST_CASE("Task 2: edge cases", "[task2]") {
    SECTION("empty input has no duplicates") {
        checkAll(std::vector<int>{}, false);
    }
    SECTION("a single element has no duplicates") {
        checkAll(std::vector<int>{7}, false);
    }
    SECTION("two equal elements") {
        checkAll(std::vector<int>{7, 7}, true);
    }
    SECTION("duplicate at the very ends") {
        checkAll(std::vector<int>{1, 2, 3, 4, 1}, true);
    }
    SECTION("all distinct") {
        checkAll(std::vector<int>{5, 3, 9, -1, 0, 42}, false);
    }
    SECTION("negative numbers") {
        checkAll(std::vector<int>{-3, -2, -1, -2}, true);
    }
}

TEST_CASE("Task 2: works for any comparable and hashable type", "[task2]") {
    checkAll(std::vector<std::string>{"tree", "heap", "graph"}, false);
    checkAll(std::vector<std::string>{"tree", "heap", "tree"}, true);
    checkAll(std::vector<char>{'a', 'b', 'c', 'a'}, true);
}

TEST_CASE("Task 2: hasDuplicateSorting does not modify the caller's data", "[task2]") {
    const std::vector<int> original{3, 1, 2};
    std::vector<int> copy = original;
    hasDuplicateSorting(copy);
    REQUIRE(copy == original);
}

TEST_CASE("Task 2: random inputs - all three agree", "[task2]") {
    std::mt19937 rng(2026);
    for (int round = 0; round < 200; ++round) {
        const int size = static_cast<int>(rng() % 60);
        const int range = 1 + static_cast<int>(rng() % 100);
        std::vector<int> values(static_cast<std::size_t>(size));
        for (int& v : values) {
            v = static_cast<int>(rng() % static_cast<unsigned>(range));
        }
        // Brute-force reference, written independently of your code.
        bool expected = false;
        for (std::size_t i = 0; i < values.size() && !expected; ++i) {
            for (std::size_t j = i + 1; j < values.size(); ++j) {
                if (values[i] == values[j]) {
                    expected = true;
                    break;
                }
            }
        }
        INFO("round " << round << ", size " << size << ", range " << range);
        checkAll(values, expected);
    }
}

// Task 4: add your own TEST_CASE below. Ideas: a large input with exactly
// one duplicate; a vector of doubles containing both 0.0 and -0.0
// (are they duplicates? what does each implementation say?).
