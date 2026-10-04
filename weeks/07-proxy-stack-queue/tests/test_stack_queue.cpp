#include <catch_amalgamated.hpp>

#include <deque>
#include <list>
#include <memory>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>

#include "ring_queue.h"
#include "stack.h"
#include "two_stack_queue.h"

// ---------------------------------------------------------------- Task 1

TEMPLATE_TEST_CASE("Task 1: Stack over different containers", "[task1]", std::vector<int>, std::deque<int>,
                   std::list<int>) {
    Stack<int, TestType> s;
    REQUIRE(s.empty());
    for (int i = 1; i <= 5; ++i) s.push(i);
    REQUIRE(s.size() == 5);
    REQUIRE(s.top() == 5);  // last in...
    s.pop();
    REQUIRE(s.top() == 4);  // ...first out
    s.top() = 40;
    REQUIRE(s.top() == 40);
    while (!s.empty()) s.pop();
    REQUIRE_THROWS_AS(s.top(), std::out_of_range);
    REQUIRE_THROWS_AS(s.pop(), std::out_of_range);
}

TEST_CASE("Task 1: push(T&&) works with move-only types", "[task1]") {
    Stack<std::unique_ptr<int>> s;
    s.push(std::make_unique<int>(7));  // cannot be copied - only moved
    auto p = std::make_unique<int>(8);
    s.push(std::move(p));
    REQUIRE(*s.top() == 8);
    s.pop();
    REQUIRE(*s.top() == 7);
    const Stack<std::unique_ptr<int>>& view = s;
    REQUIRE(*view.top() == 7);
}

TEST_CASE("Task 1: reversing a string with a stack", "[task1]") {
    Stack<char> s;
    for (char c : std::string("stack")) s.push(c);
    std::string out;
    while (!s.empty()) {
        out += s.top();
        s.pop();
    }
    REQUIRE(out == "kcats");
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: RingQueue is first-in-first-out", "[task2]") {
    RingQueue<std::string> q;
    REQUIRE(q.empty());
    REQUIRE_THROWS_AS(q.front(), std::out_of_range);
    REQUIRE_THROWS_AS(q.pop(), std::out_of_range);
    q.push("a");
    q.push("b");
    q.push("c");
    REQUIRE(q.front() == "a");
    REQUIRE(q.back() == "c");
    q.pop();
    REQUIRE(q.front() == "b");
    REQUIRE(q.size() == 2);
}

TEST_CASE("Task 2: RingQueue wraps around instead of growing", "[task2]") {
    RingQueue<int> q;
    for (int i = 0; i < 4; ++i) q.push(i);
    REQUIRE(q.capacity() == 4);
    // A steady stream of push + pop: the 4 slots are reused forever.
    for (int i = 4; i < 1000; ++i) {
        q.pop();
        q.push(i);
        REQUIRE(q.capacity() == 4);
        REQUIRE(q.front() == i - 3);
        REQUIRE(q.back() == i);
    }
}

TEST_CASE("Task 2: RingQueue grows while wrapped, keeping the order", "[task2]") {
    RingQueue<int> q;
    for (int i = 0; i < 8; ++i) q.push(i);  // capacity 8
    for (int i = 0; i < 5; ++i) q.pop();    // head in the middle
    for (int i = 8; i < 13; ++i) q.push(i); // wraps around: full again
    REQUIRE(q.size() == 8);
    REQUIRE(q.capacity() == 8);
    q.push(13);                             // grow while wrapped
    REQUIRE(q.capacity() == 16);
    for (std::size_t i = 0; i < q.size(); ++i) {
        INFO("i = " << i);
        REQUIRE(q.at(i) == static_cast<int>(5 + i));
    }
}

TEST_CASE("Task 2: random operations behave like std::queue", "[task2]") {
    std::mt19937 rng(2026);
    RingQueue<int> q;
    std::queue<int> model;
    for (int step = 0; step < 20000; ++step) {
        if (model.empty() || rng() % 5 < 3) {
            const int v = static_cast<int>(rng() % 1000);
            q.push(v);
            model.push(v);
        } else {
            REQUIRE(q.front() == model.front());
            q.pop();
            model.pop();
        }
        REQUIRE(q.size() == model.size());
        if (!model.empty()) {
            REQUIRE(q.front() == model.front());
            REQUIRE(q.back() == model.back());
        }
    }
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: TwoStackQueue is first-in-first-out", "[task3]") {
    TwoStackQueue<int> q;
    REQUIRE_THROWS_AS(q.front(), std::out_of_range);
    q.push(1);
    q.push(2);
    q.push(3);
    REQUIRE(q.front() == 1);
    q.pop();
    q.push(4);  // goes to in_, behind 2 and 3
    REQUIRE(q.front() == 2);
    q.pop();
    q.pop();
    REQUIRE(q.front() == 4);
    q.pop();
    REQUIRE(q.empty());
    REQUIRE_THROWS_AS(q.pop(), std::out_of_range);
}

TEST_CASE("Task 3: every element is transferred at most once", "[task3]") {
    std::mt19937 rng(7);
    TwoStackQueue<int> q;
    std::queue<int> model;
    long pushes = 0;
    for (int step = 0; step < 20000; ++step) {
        if (model.empty() || rng() % 2 == 0) {
            q.push(step);
            model.push(step);
            ++pushes;
        } else {
            REQUIRE(q.front() == model.front());
            q.pop();
            model.pop();
        }
    }
    INFO("transfers " << q.transfers() << ", pushes " << pushes);
    REQUIRE(q.transfers() <= pushes);  // amortized Theta(1)
}
