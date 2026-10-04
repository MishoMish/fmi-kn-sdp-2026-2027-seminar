#include <catch_amalgamated.hpp>

#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

#include "applications.h"

TEST_CASE("Task 5: balancedBrackets", "[task5]") {
    REQUIRE(balancedBrackets(""));
    REQUIRE(balancedBrackets("()"));
    REQUIRE(balancedBrackets("([]{})"));
    REQUIRE(balancedBrackets("f(a[i], {x, y}) + (b)"));
    REQUIRE(balancedBrackets("(((([[[{}]]]))))"));
    REQUIRE_FALSE(balancedBrackets("("));
    REQUIRE_FALSE(balancedBrackets(")"));
    REQUIRE_FALSE(balancedBrackets("([)]"));
    REQUIRE_FALSE(balancedBrackets("(()"));
    REQUIRE_FALSE(balancedBrackets("())("));
    REQUIRE_FALSE(balancedBrackets("{]"));
}

TEST_CASE("Task 5: MinStack", "[task5]") {
    MinStack<int> s;
    REQUIRE_THROWS_AS(s.min(), std::out_of_range);
    s.push(5);
    REQUIRE(s.min() == 5);
    s.push(3);
    s.push(7);
    REQUIRE(s.min() == 3);
    s.push(3);  // a repeated minimum
    s.push(1);
    REQUIRE(s.min() == 1);
    s.pop();
    REQUIRE(s.min() == 3);
    s.pop();
    REQUIRE(s.min() == 3);  // the other 3 is still there
    s.pop();
    REQUIRE(s.top() == 3);
    s.pop();
    REQUIRE(s.min() == 5);
    REQUIRE(s.top() == 5);
    s.pop();
    REQUIRE(s.empty());
    REQUIRE_THROWS_AS(s.pop(), std::out_of_range);
}

TEST_CASE("Task 5: MinStack against brute force", "[task5]") {
    std::mt19937 rng(2026);
    MinStack<int> s;
    std::vector<int> model;
    for (int step = 0; step < 5000; ++step) {
        if (model.empty() || rng() % 3 != 0) {
            const int v = static_cast<int>(rng() % 100);
            s.push(v);
            model.push_back(v);
        } else {
            s.pop();
            model.pop_back();
        }
        if (!model.empty()) {
            REQUIRE(s.min() == *std::min_element(model.begin(), model.end()));
            REQUIRE(s.top() == model.back());
        }
    }
}

TEST_CASE("Task 5: slidingWindowMax", "[task5]") {
    REQUIRE(slidingWindowMax({1, 3, -1, -3, 5, 3, 6, 7}, 3) == std::vector<int>{3, 3, 5, 5, 6, 7});
    REQUIRE(slidingWindowMax({4, 2}, 1) == std::vector<int>{4, 2});
    REQUIRE(slidingWindowMax({4, 2, 9}, 3) == std::vector<int>{9});
    REQUIRE(slidingWindowMax({9, 8, 7, 6}, 2) == std::vector<int>{9, 8, 7});
    REQUIRE_THROWS_AS(slidingWindowMax({1, 2}, 0), std::invalid_argument);
    REQUIRE_THROWS_AS(slidingWindowMax({1, 2}, 3), std::invalid_argument);

    std::mt19937 rng(2026);
    for (int round = 0; round < 300; ++round) {
        std::vector<int> a(1 + rng() % 40);
        for (int& x : a) x = static_cast<int>(rng() % 20);
        const std::size_t k = 1 + rng() % a.size();
        std::vector<int> expected;
        for (std::size_t i = 0; i + k <= a.size(); ++i) {
            expected.push_back(*std::max_element(a.begin() + static_cast<long>(i),
                                                 a.begin() + static_cast<long>(i + k)));
        }
        INFO("round " << round << ", k " << k);
        REQUIRE(slidingWindowMax(a, k) == expected);
    }
}
