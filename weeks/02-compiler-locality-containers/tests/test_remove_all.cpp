#include <catch_amalgamated.hpp>

#include <deque>
#include <list>
#include <random>
#include <string>
#include <vector>

#include "remove_all.h"

namespace {

// Same checks for both implementations, on any container type.
template <typename Container>
void checkBoth(const Container& input, const typename Container::value_type& value,
               const Container& expected) {
    Container a = input;
    Container b = input;
    const std::size_t removed = input.size() - expected.size();
    CHECK(removeAllNaive(a, value) == removed);
    CHECK(a == expected);
    CHECK(removeAllLinear(b, value) == removed);
    CHECK(b == expected);
}

}  // namespace

TEST_CASE("Task 3: std::vector", "[task3]") {
    using V = std::vector<int>;
    checkBoth(V{}, 1, V{});
    checkBoth(V{1, 2, 3}, 7, V{1, 2, 3});
    checkBoth(V{7, 7, 7}, 7, V{});
    checkBoth(V{7, 1, 7, 2, 7}, 7, V{1, 2});
    checkBoth(V{1, 7, 7, 2}, 7, V{1, 2});  // adjacent matches: the classic iterator bug
}

TEST_CASE("Task 3: other sequence containers", "[task3]") {
    checkBoth(std::deque<int>{3, 1, 3, 3, 2}, 3, std::deque<int>{1, 2});
    checkBoth(std::list<int>{5, 5, 1, 5}, 5, std::list<int>{1});
    checkBoth(std::string("mississippi"), 's', std::string("miiippi"));
    checkBoth(std::vector<std::string>{"a", "b", "a"}, std::string("a"),
              std::vector<std::string>{"b"});
}

TEST_CASE("Task 3: random inputs keep the order of the rest", "[task3]") {
    std::mt19937 rng(2026);
    for (int round = 0; round < 200; ++round) {
        std::vector<int> input(rng() % 40);
        for (int& x : input) {
            x = static_cast<int>(rng() % 4);
        }
        const int value = static_cast<int>(rng() % 4);
        std::vector<int> expected;
        for (int x : input) {
            if (x != value) {
                expected.push_back(x);
            }
        }
        INFO("round " << round);
        checkBoth(input, value, expected);
    }
}
