#include <catch_amalgamated.hpp>

#include <algorithm>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "fixed_array.h"

namespace {

template <typename T>
std::vector<T> contents(const FixedArray<T>& a) {
    return std::vector<T>(a.begin(), a.end());
}

}  // namespace

TEST_CASE("Task 1: a new array is empty", "[task1]") {
    const FixedArray<int> a(5);
    REQUIRE(a.size() == 0);
    REQUIRE(a.capacity() == 5);
    REQUIRE(a.empty());
    REQUIRE_FALSE(a.full());
    REQUIRE(a.begin() == a.end());
}

TEST_CASE("Task 1: pushBack and popBack", "[task1]") {
    FixedArray<int> a(3);
    a.pushBack(10);
    a.pushBack(20);
    REQUIRE(a.size() == 2);
    REQUIRE(contents(a) == std::vector<int>{10, 20});
    a.pushBack(30);
    REQUIRE(a.full());
    REQUIRE_THROWS_AS(a.pushBack(40), std::length_error);
    REQUIRE(a.size() == 3);  // a failed operation changes nothing

    a.popBack();
    REQUIRE(contents(a) == std::vector<int>{10, 20});
    a.popBack();
    a.popBack();
    REQUIRE(a.empty());
    REQUIRE_THROWS_AS(a.popBack(), std::out_of_range);
}

TEST_CASE("Task 1: at() checks the size, not the capacity", "[task1]") {
    FixedArray<int> a(10, {1, 2, 3});
    REQUIRE(a.at(0) == 1);
    REQUIRE(a.at(2) == 3);
    a.at(1) = 42;
    REQUIRE(a[1] == 42);
    REQUIRE_THROWS_AS(a.at(3), std::out_of_range);    // inside capacity, outside size
    REQUIRE_THROWS_AS(a.at(100), std::out_of_range);

    const FixedArray<int>& view = a;
    REQUIRE(view.at(1) == 42);
    REQUIRE_THROWS_AS(view.at(3), std::out_of_range);
}

TEST_CASE("Task 2: insertAt shifts the tail right", "[task2]") {
    FixedArray<std::string> a(6, {"b", "d"});
    a.insertAt(0, "a");                 // front
    a.insertAt(2, "c");                 // middle
    a.insertAt(a.size(), "e");          // end
    REQUIRE(contents(a) == std::vector<std::string>{"a", "b", "c", "d", "e"});
    REQUIRE_THROWS_AS(a.insertAt(7, "x"), std::out_of_range);
    a.insertAt(5, "f");
    REQUIRE(a.full());
    REQUIRE_THROWS_AS(a.insertAt(0, "x"), std::length_error);
    REQUIRE(contents(a) == std::vector<std::string>{"a", "b", "c", "d", "e", "f"});
}

TEST_CASE("Task 2: removeAt shifts the tail left", "[task2]") {
    FixedArray<int> a(5, {1, 2, 3, 4, 5});
    a.removeAt(0);
    REQUIRE(contents(a) == std::vector<int>{2, 3, 4, 5});
    a.removeAt(1);
    REQUIRE(contents(a) == std::vector<int>{2, 4, 5});
    a.removeAt(a.size() - 1);
    REQUIRE(contents(a) == std::vector<int>{2, 4});
    REQUIRE_THROWS_AS(a.removeAt(2), std::out_of_range);
}

TEST_CASE("Task 2: random operations behave like std::vector", "[task2]") {
    std::mt19937 rng(2026);
    const std::size_t capacity = 20;
    FixedArray<int> a(capacity);
    std::vector<int> model;
    for (int step = 0; step < 2000; ++step) {
        const int op = static_cast<int>(rng() % 4);
        const int value = static_cast<int>(rng() % 100);
        INFO("step " << step << ", op " << op);
        if (op == 0 && model.size() < capacity) {
            a.pushBack(value);
            model.push_back(value);
        } else if (op == 1 && !model.empty()) {
            a.popBack();
            model.pop_back();
        } else if (op == 2 && model.size() < capacity) {
            const std::size_t i = rng() % (model.size() + 1);
            a.insertAt(i, value);
            model.insert(model.begin() + static_cast<std::ptrdiff_t>(i), value);
        } else if (op == 3 && !model.empty()) {
            const std::size_t i = rng() % model.size();
            a.removeAt(i);
            model.erase(model.begin() + static_cast<std::ptrdiff_t>(i));
        }
        REQUIRE(contents(a) == model);
    }
}

TEST_CASE("Task 2: works with standard algorithms through begin()/end()", "[task2]") {
    FixedArray<int> a(8, {5, 3, 8, 1});
    std::sort(a.begin(), a.end());
    REQUIRE(contents(a) == std::vector<int>{1, 3, 5, 8});
    REQUIRE(std::accumulate(a.begin(), a.end(), 0) == 17);
    REQUIRE(std::find(a.begin(), a.end(), 8) - a.begin() == 3);
}

TEST_CASE("Task 3: the copy constructor makes an independent copy", "[task3]") {
    FixedArray<int> a(4, {1, 2, 3});
    FixedArray<int> b(a);
    REQUIRE(b == a);
    REQUIRE(b.capacity() == 4);
    REQUIRE(b.begin() != a.begin());  // its own buffer
    b[0] = 100;
    b.pushBack(4);
    REQUIRE(contents(a) == std::vector<int>{1, 2, 3});
    REQUIRE(contents(b) == std::vector<int>{100, 2, 3, 4});
}

TEST_CASE("Task 3: copy assignment", "[task3]") {
    FixedArray<std::string> a(2, {"x", "y"});
    FixedArray<std::string> b(5, {"p"});
    b = a;
    REQUIRE(b == a);
    REQUIRE(b.capacity() == 2);
    REQUIRE(b.begin() != a.begin());
    b[0] = "changed";
    REQUIRE(a[0] == "x");

    SECTION("self-assignment keeps the contents") {
        a = a;
        REQUIRE(contents(a) == std::vector<std::string>{"x", "y"});
    }
    SECTION("assignment returns *this, so it chains") {
        FixedArray<std::string> c(1);
        c = b = a;
        REQUIRE(c == a);
    }
}
