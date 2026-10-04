#include <catch_amalgamated.hpp>

#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "dynamic_array.h"
#include "tracked.h"

namespace {

template <typename T>
std::vector<T> contents(const DynamicArray<T>& a) {
    return std::vector<T>(a.begin(), a.end());
}

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: a new array has no buffer", "[task1]") {
    const DynamicArray<int> a;
    REQUIRE(a.size() == 0);
    REQUIRE(a.capacity() == 0);
    REQUIRE(a.data() == nullptr);
}

TEST_CASE("Task 1: pushBack doubles the capacity", "[task1]") {
    DynamicArray<int> a;
    std::vector<std::size_t> capacities;
    for (int i = 0; i < 17; ++i) {
        a.pushBack(i);
        capacities.push_back(a.capacity());
    }
    REQUIRE(capacities == std::vector<std::size_t>{1, 2, 4, 4, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16,
                                                   16, 16, 32});
    for (int i = 0; i < 17; ++i) {
        REQUIRE(a[static_cast<std::size_t>(i)] == i);
    }
}

TEST_CASE("Task 1: reserve", "[task1]") {
    DynamicArray<std::string> a;
    a.reserve(10);
    REQUIRE(a.capacity() == 10);
    REQUIRE(a.size() == 0);
    a.pushBack("x");
    const std::string* buffer = a.data();
    for (int i = 0; i < 9; ++i) {
        a.pushBack("y");
    }
    REQUIRE(a.data() == buffer);  // 10 elements fit: no reallocation
    a.reserve(5);                 // never shrinks
    REQUIRE(a.capacity() == 10);
    a.reserve(12);
    REQUIRE(a.capacity() == 12);
    REQUIRE(a.size() == 10);
    REQUIRE(a[0] == "x");
    REQUIRE(a[9] == "y");
}

TEST_CASE("Task 1: popBack", "[task1]") {
    DynamicArray<int> a{1, 2, 3};
    a.popBack();
    REQUIRE(contents(a) == std::vector<int>{1, 2});
    REQUIRE(a.capacity() >= 2);  // popBack does not shrink
    a.popBack();
    a.popBack();
    REQUIRE(a.empty());
    REQUIRE_THROWS_AS(a.popBack(), std::out_of_range);
}

TEST_CASE("Task 1: pushBack of an element of the same array", "[task1]") {
    // Run this one with the asan preset: a wrong implementation reads the
    // old buffer after freeing it, which may "work" by luck without ASan.
    DynamicArray<std::string> a;
    a.pushBack("first element, long enough to live on the heap");
    a.pushBack("second");
    REQUIRE(a.size() == a.capacity());  // full: the next pushBack reallocates
    a.pushBack(a[0]);
    REQUIRE(a.size() == 3);
    REQUIRE(a[2] == "first element, long enough to live on the heap");
    REQUIRE(a[0] == a[2]);
}

TEST_CASE("Task 1: n pushBacks cost O(n) element transfers in total", "[task1]") {
    Tracked::counters.reset();
    DynamicArray<Tracked> a;
    const int n = 1000;
    for (int i = 0; i < n; ++i) {
        const Tracked t(i);
        a.pushBack(t);
    }
    const long transfers = Tracked::counters.copies + Tracked::counters.moves;
    INFO("copies " << Tracked::counters.copies << ", moves " << Tracked::counters.moves);
    // n to put the new elements in + 1 + 2 + 4 + ... + 512 = 1023 for regrowth
    REQUIRE(transfers <= 3 * n);
    for (int i = 0; i < n; ++i) {
        REQUIRE(a[static_cast<std::size_t>(i)].value == i);
    }
}

TEST_CASE("Task 1: random pushBack/popBack behave like std::vector", "[task1]") {
    std::mt19937 rng(2026);
    DynamicArray<int> a;
    std::vector<int> model;
    for (int step = 0; step < 5000; ++step) {
        if (model.empty() || rng() % 3 != 0) {
            const int v = static_cast<int>(rng() % 1000);
            a.pushBack(v);
            model.push_back(v);
        } else {
            a.popBack();
            model.pop_back();
        }
        INFO("step " << step);
        REQUIRE(a.size() == model.size());
        REQUIRE(a.capacity() >= a.size());
    }
    REQUIRE(contents(a) == model);
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: copy constructor and copy assignment are deep", "[task2]") {
    DynamicArray<std::string> a{"tree", "heap", "graph"};
    DynamicArray<std::string> b(a);
    REQUIRE(b == a);
    REQUIRE(b.data() != a.data());
    b[0] = "changed";
    REQUIRE(a[0] == "tree");

    DynamicArray<std::string> c{"x"};
    c = a;
    REQUIRE(c == a);
    REQUIRE(c.data() != a.data());
    c = c;  // self-assignment
    REQUIRE(c == a);

    const DynamicArray<std::string> empty;
    DynamicArray<std::string> d(empty);
    REQUIRE(d.empty());
}

TEST_CASE("Task 2: move constructor steals the buffer", "[task2]") {
    Tracked::counters.reset();
    DynamicArray<Tracked> a{1, 2, 3};
    const Tracked* buffer = a.data();
    Tracked::counters.reset();

    DynamicArray<Tracked> b(std::move(a));
    REQUIRE(b.data() == buffer);  // the same buffer: nothing was allocated
    REQUIRE(b.size() == 3);
    REQUIRE(Tracked::counters.copies == 0);
    REQUIRE(Tracked::counters.moves == 0);  // not even element moves
    REQUIRE(a.size() == 0);                 // NOLINT: moved-from is empty...
    REQUIRE(a.capacity() == 0);
    REQUIRE(a.data() == nullptr);
    a.pushBack(7);                          // ...and still usable
    REQUIRE(a[0].value == 7);
}

TEST_CASE("Task 2: move assignment", "[task2]") {
    DynamicArray<std::string> a{"a", "b"};
    DynamicArray<std::string> b{"old", "contents", "here"};
    const std::string* buffer = a.data();
    b = std::move(a);
    REQUIRE(b.data() == buffer);
    REQUIRE(contents(b) == std::vector<std::string>{"a", "b"});
    REQUIRE(a.empty());  // NOLINT
    REQUIRE(a.capacity() == 0);
}

TEST_CASE("Task 2: move operations are noexcept", "[task2]") {
    STATIC_REQUIRE(std::is_nothrow_move_constructible<DynamicArray<std::string>>::value);
    STATIC_REQUIRE(std::is_nothrow_move_assignable<DynamicArray<std::string>>::value);
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: pushBack(T&&) moves instead of copying", "[task3]") {
    DynamicArray<Tracked> a;
    a.reserve(4);
    Tracked::counters.reset();
    Tracked t(5);
    a.pushBack(std::move(t));
    REQUIRE(Tracked::counters.copies == 0);
    REQUIRE(Tracked::counters.moves == 1);
    a.pushBack(t);  // an lvalue: copied
    REQUIRE(Tracked::counters.copies == 1);
}

TEST_CASE("Task 3: resize", "[task3]") {
    DynamicArray<int> a{1, 2, 3};
    a.resize(5);
    REQUIRE(contents(a) == std::vector<int>{1, 2, 3, 0, 0});
    a.resize(2);
    REQUIRE(contents(a) == std::vector<int>{1, 2});
    REQUIRE(a.capacity() >= 5);  // shrinking the size keeps the capacity
    a.resize(4);
    REQUIRE(contents(a) == std::vector<int>{1, 2, 0, 0});  // old 3 is gone
    a.resize(0);
    REQUIRE(a.empty());
}

TEST_CASE("Task 3: shrinkToFit", "[task3]") {
    DynamicArray<int> a;
    for (int i = 0; i < 9; ++i) {
        a.pushBack(i);
    }
    REQUIRE(a.capacity() == 16);
    a.shrinkToFit();
    REQUIRE(a.capacity() == 9);
    REQUIRE(a.size() == 9);
    REQUIRE(a[8] == 8);
    while (!a.empty()) {
        a.popBack();
    }
    a.shrinkToFit();
    REQUIRE(a.capacity() == 0);
    REQUIRE(a.data() == nullptr);
}

// ---------------------------------------------------------------- Task 4

// Task 4 needs Task 3: the new element is a temporary, moved in exactly once.
TEST_CASE("Task 4: regrowth moves noexcept-movable elements", "[task4]") {
    DynamicArray<Tracked> a;
    for (int i = 0; i < 8; ++i) {
        a.pushBack(Tracked(i));  // capacity 8, full
    }
    Tracked::counters.reset();
    a.pushBack(Tracked(8));  // regrowth: 8 old elements + the new one
    REQUIRE(Tracked::counters.copies == 0);
    REQUIRE(Tracked::counters.moves == 9);
}

TEST_CASE("Task 4: regrowth copies elements whose move may throw", "[task4]") {
    DynamicArray<ThrowingMove> a;
    for (int i = 0; i < 8; ++i) {
        a.pushBack(ThrowingMove(i));
    }
    ThrowingMove::counters.reset();
    a.pushBack(ThrowingMove(8));
    // The 8 old elements are copied (strong exception guarantee); only the
    // new element, a temporary, is moved in.
    REQUIRE(ThrowingMove::counters.copies == 8);
    REQUIRE(ThrowingMove::counters.moves == 1);
    for (int i = 0; i < 9; ++i) {
        REQUIRE(a[static_cast<std::size_t>(i)].value == i);
    }
}
