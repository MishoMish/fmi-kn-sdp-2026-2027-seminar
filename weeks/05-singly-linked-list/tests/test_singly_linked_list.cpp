#include <catch_amalgamated.hpp>

#include <list>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "singly_linked_list.h"

namespace {

template <typename T>
std::vector<T> contents(const SinglyLinkedList<T>& list) {
    std::vector<T> out;
    list.forEach([&](const T& v) { out.push_back(v); });
    return out;
}

// Checks the invariant from the outside: size, front and back agree with
// the elements visited by forEach.
template <typename T>
void checkInvariant(const SinglyLinkedList<T>& list) {
    const auto v = contents(list);
    REQUIRE(v.size() == list.size());
    REQUIRE(list.empty() == v.empty());
    if (!v.empty()) {
        REQUIRE(list.front() == v.front());
        REQUIRE(list.back() == v.back());
    }
}

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: pushBack and pushFront", "[task1]") {
    SinglyLinkedList<int> list;
    list.pushBack(2);
    list.pushBack(3);
    list.pushFront(1);
    list.pushFront(0);
    REQUIRE(contents(list) == std::vector<int>{0, 1, 2, 3});
    checkInvariant(list);
}

TEST_CASE("Task 1: the first push into an empty list sets both ends", "[task1]") {
    SinglyLinkedList<int> a;
    a.pushFront(7);
    REQUIRE(a.front() == 7);
    REQUIRE(a.back() == 7);  // tail_ must be set too
    a.pushBack(8);
    REQUIRE(contents(a) == std::vector<int>{7, 8});

    SinglyLinkedList<int> b;
    b.pushBack(7);
    b.pushFront(6);  // head_ must have been set by pushBack
    REQUIRE(contents(b) == std::vector<int>{6, 7});
}

TEST_CASE("Task 1: popFront down to empty and back", "[task1]") {
    SinglyLinkedList<std::string> list{"a", "b"};
    list.popFront();
    REQUIRE(contents(list) == std::vector<std::string>{"b"});
    list.popFront();
    REQUIRE(list.empty());
    REQUIRE_THROWS_AS(list.popFront(), std::out_of_range);
    REQUIRE_THROWS_AS(list.front(), std::out_of_range);
    list.pushBack("c");  // tail_ must have been reset, or this writes through a dead node
    REQUIRE(contents(list) == std::vector<std::string>{"c"});
    checkInvariant(list);
}

TEST_CASE("Task 1: clear, and a long list", "[task1]") {
    SinglyLinkedList<int> list;
    for (int i = 0; i < 1'000'000; ++i) {
        list.pushFront(i);
    }
    REQUIRE(list.size() == 1'000'000);
    REQUIRE(list.front() == 999'999);
    list.clear();
    REQUIRE(list.empty());
    list.pushBack(1);
    REQUIRE(list.front() == 1);
    // destructor of a million-node list: must not overflow the stack
    SinglyLinkedList<int> big;
    for (int i = 0; i < 1'000'000; ++i) {
        big.pushBack(i);
    }
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: insertAt", "[task2]") {
    SinglyLinkedList<int> list;
    list.insertAt(0, 2);  // into an empty list
    list.insertAt(0, 0);  // front
    list.insertAt(1, 1);  // middle
    list.insertAt(3, 3);  // end
    REQUIRE(contents(list) == std::vector<int>{0, 1, 2, 3});
    REQUIRE(list.back() == 3);
    REQUIRE_THROWS_AS(list.insertAt(5, 9), std::out_of_range);
    list.pushBack(4);  // tail_ was updated by insertAt at the end
    REQUIRE(contents(list) == std::vector<int>{0, 1, 2, 3, 4});
}

TEST_CASE("Task 2: removeAt", "[task2]") {
    SinglyLinkedList<int> list{0, 1, 2, 3, 4};
    list.removeAt(0);
    list.removeAt(1);
    REQUIRE(contents(list) == std::vector<int>{1, 3, 4});
    list.removeAt(2);  // the last node: tail_ must move back
    REQUIRE(list.back() == 3);
    list.pushBack(5);
    REQUIRE(contents(list) == std::vector<int>{1, 3, 5});
    REQUIRE_THROWS_AS(list.removeAt(3), std::out_of_range);
    list.removeAt(0);
    list.removeAt(0);
    list.removeAt(0);
    REQUIRE(list.empty());
    list.pushBack(9);
    REQUIRE(contents(list) == std::vector<int>{9});
}

TEST_CASE("Task 2: indexOf", "[task2]") {
    const SinglyLinkedList<std::string> list{"x", "y", "x"};
    REQUIRE(list.indexOf("x") == 0);
    REQUIRE(list.indexOf("y") == 1);
    REQUIRE(list.indexOf("z") == SinglyLinkedList<std::string>::npos);
    REQUIRE(SinglyLinkedList<int>{}.indexOf(1) == SinglyLinkedList<int>::npos);
}

TEST_CASE("Task 2: random operations behave like std::list", "[task2]") {
    std::mt19937 rng(2026);
    SinglyLinkedList<int> list;
    std::list<int> model;
    for (int step = 0; step < 3000; ++step) {
        const int op = static_cast<int>(rng() % 5);
        const int v = static_cast<int>(rng() % 50);
        INFO("step " << step << ", op " << op);
        if (op == 0) {
            list.pushFront(v);
            model.push_front(v);
        } else if (op == 1) {
            list.pushBack(v);
            model.push_back(v);
        } else if (op == 2 && !model.empty()) {
            list.popFront();
            model.pop_front();
        } else if (op == 3) {
            const std::size_t i = rng() % (model.size() + 1);
            list.insertAt(i, v);
            model.insert(std::next(model.begin(), static_cast<long>(i)), v);
        } else if (op == 4 && !model.empty()) {
            const std::size_t i = rng() % model.size();
            list.removeAt(i);
            model.erase(std::next(model.begin(), static_cast<long>(i)));
        }
        REQUIRE(contents(list) == std::vector<int>(model.begin(), model.end()));
        checkInvariant(list);
    }
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: copy keeps the order and is independent", "[task3]") {
    SinglyLinkedList<std::string> a{"one", "two", "three"};
    SinglyLinkedList<std::string> b(a);
    REQUIRE(b == a);
    b.pushBack("four");
    b.front() = "ONE";
    REQUIRE(contents(a) == std::vector<std::string>{"one", "two", "three"});
    REQUIRE(contents(b) == std::vector<std::string>{"ONE", "two", "three", "four"});
    checkInvariant(b);

    SinglyLinkedList<std::string> c{"old"};
    c = a;
    REQUIRE(c == a);
    c = c;
    REQUIRE(c == a);
    c.pushBack("x");  // tail_ of the copy points into the copy
    REQUIRE(a.size() == 3);
}

TEST_CASE("Task 3: copying a long list is linear", "[task3]") {
    SinglyLinkedList<int> a;
    for (int i = 0; i < 200'000; ++i) {
        a.pushBack(i);
    }
    // Theta(n^2) here (e.g. walking to the end for every element) takes
    // minutes; Theta(n) takes milliseconds.
    const SinglyLinkedList<int> b(a);
    REQUIRE(b.size() == 200'000);
    REQUIRE(b.back() == 199'999);
}

TEST_CASE("Task 3: move steals the nodes", "[task3]") {
    SinglyLinkedList<int> a{1, 2, 3};
    SinglyLinkedList<int> b(std::move(a));
    REQUIRE(contents(b) == std::vector<int>{1, 2, 3});
    REQUIRE(a.empty());  // NOLINT
    a.pushBack(4);       // still usable
    REQUIRE(contents(a) == std::vector<int>{4});

    SinglyLinkedList<int> c{9, 9};
    c = std::move(b);
    REQUIRE(contents(c) == std::vector<int>{1, 2, 3});
    REQUIRE(b.empty());  // NOLINT
    STATIC_REQUIRE(std::is_nothrow_move_constructible<SinglyLinkedList<int>>::value);
}

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: reverse", "[task4]") {
    SinglyLinkedList<int> list{1, 2, 3, 4, 5};
    list.reverse();
    REQUIRE(contents(list) == std::vector<int>{5, 4, 3, 2, 1});
    REQUIRE(list.front() == 5);
    REQUIRE(list.back() == 1);
    list.pushBack(0);  // tail_ must be the old head
    REQUIRE(contents(list) == std::vector<int>{5, 4, 3, 2, 1, 0});

    SinglyLinkedList<int> empty;
    empty.reverse();
    REQUIRE(empty.empty());
    SinglyLinkedList<int> one{7};
    one.reverse();
    REQUIRE(contents(one) == std::vector<int>{7});
    checkInvariant(one);
}

TEST_CASE("Task 4: reverse relinks nodes instead of copying values", "[task4]") {
    SinglyLinkedList<std::string> list{"first", "middle", "last"};
    const std::string* first = &list.front();
    const std::string* last = &list.back();
    list.reverse();
    // The same objects, now at the other ends: nothing was copied or moved.
    REQUIRE(&list.back() == first);
    REQUIRE(&list.front() == last);
}

TEST_CASE("Task 4: removeAll", "[task4]") {
    SinglyLinkedList<int> list{7, 1, 7, 7, 2, 7};
    REQUIRE(list.removeAll(7) == 4);
    REQUIRE(contents(list) == std::vector<int>{1, 2});
    REQUIRE(list.back() == 2);
    list.pushBack(3);
    REQUIRE(contents(list) == std::vector<int>{1, 2, 3});
    REQUIRE(list.removeAll(9) == 0);

    SinglyLinkedList<int> all{5, 5, 5};
    REQUIRE(all.removeAll(5) == 3);
    REQUIRE(all.empty());
    all.pushBack(1);
    REQUIRE(contents(all) == std::vector<int>{1});
}
