#include <catch_amalgamated.hpp>

#include <algorithm>
#include <iterator>
#include <list>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "counted.h"
#include "list.h"

namespace {

template <typename T>
std::vector<T> forwards(const List<T>& list) {
    return std::vector<T>(list.begin(), list.end());
}

// Walks the list backwards with --, from end() to begin().
template <typename T>
std::vector<T> backwards(const List<T>& list) {
    std::vector<T> out;
    for (auto it = list.end(); it != list.begin();) {
        --it;
        out.push_back(*it);
    }
    return out;
}

// Both directions must agree, and agree with size(): this catches a
// forgotten prev pointer immediately.
template <typename T>
void checkLinks(const List<T>& list) {
    auto f = forwards(list);
    auto b = backwards(list);
    std::reverse(b.begin(), b.end());
    REQUIRE(f == b);
    REQUIRE(f.size() == list.size());
}

}  // namespace

// ---------------------------------------------------------------- Task 1 + 2

TEST_CASE("Task 1: an empty list: begin() == end()", "[task1]") {
    const List<int> list;
    REQUIRE(list.begin() == list.end());
    REQUIRE(list.size() == 0);
}

TEST_CASE("Task 1+2: iterators walk forwards and backwards", "[task1][task2]") {
    List<int> list{1, 2, 3};
    auto it = list.begin();
    REQUIRE(*it == 1);
    REQUIRE(*++it == 2);
    REQUIRE(*it++ == 2);  // post-increment returns the old position
    REQUIRE(*it == 3);
    ++it;
    REQUIRE(it == list.end());
    REQUIRE(*--it == 3);  // --end() is the last element
    REQUIRE(*it-- == 3);
    REQUIRE(*it == 2);
    checkLinks(list);
}

TEST_CASE("Task 1+2: operator-> and writing through an iterator", "[task1][task2]") {
    List<std::string> list{"tree", "heap"};
    REQUIRE(list.begin()->size() == 4);
    *list.begin() = "graph";
    REQUIRE(list.front() == "graph");
    for (std::string& s : list) {  // range-based for uses begin()/end()
        s += "!";
    }
    REQUIRE(forwards(list) == std::vector<std::string>{"graph!", "heap!"});
}

TEST_CASE("Task 1+2: const_iterator", "[task1][task2]") {
    List<int> list{4, 5};
    List<int>::const_iterator c = list.begin();  // iterator -> const_iterator
    REQUIRE(*c == 4);
    REQUIRE(c == list.begin());                  // mixed comparison
    STATIC_REQUIRE(std::is_same<decltype(*c), const int&>::value);
    STATIC_REQUIRE(!std::is_convertible<List<int>::const_iterator, List<int>::iterator>::value);
}

TEST_CASE("Task 2: insert before any position, erase anywhere", "[task2]") {
    List<int> list;
    auto it = list.insert(list.end(), 2);     // into an empty list
    REQUIRE(*it == 2);
    list.insert(list.begin(), 0);             // front
    list.insert(list.end(), 4);               // back
    auto three = list.insert(std::prev(list.end()), 3);
    list.insert(std::next(list.begin()), 1);
    REQUIRE(forwards(list) == std::vector<int>{0, 1, 2, 3, 4});
    checkLinks(list);

    auto after = list.erase(three);           // returns the next element
    REQUIRE(*after == 4);
    list.erase(list.begin());
    list.erase(std::prev(list.end()));
    REQUIRE(forwards(list) == std::vector<int>{1, 2});
    checkLinks(list);
    list.erase(list.begin());
    list.erase(list.begin());
    REQUIRE(list.empty());
    REQUIRE(list.begin() == list.end());
    list.pushBack(9);
    REQUIRE(forwards(list) == std::vector<int>{9});
}

TEST_CASE("Task 2: erase does not invalidate iterators to other elements", "[task2]") {
    List<int> list{10, 20, 30, 40};
    auto first = list.begin();
    auto last = std::prev(list.end());
    list.erase(std::next(list.begin()));      // erase 20
    list.erase(std::next(list.begin()));      // erase 30
    REQUIRE(*first == 10);                    // still valid - unlike std::vector
    REQUIRE(*last == 40);
    REQUIRE(std::next(first) == last);
}

TEST_CASE("Task 2: works with standard algorithms", "[task2]") {
    List<int> list{5, 3, 8, 3, 1};
    REQUIRE(std::count(list.begin(), list.end(), 3) == 2);
    REQUIRE(*std::find(list.begin(), list.end(), 8) == 8);
    REQUIRE(std::accumulate(list.begin(), list.end(), 0) == 20);
    REQUIRE(std::distance(list.begin(), list.end()) == 5);
    std::reverse(list.begin(), list.end());   // needs a bidirectional iterator
    REQUIRE(forwards(list) == std::vector<int>{1, 3, 8, 3, 5});
    REQUIRE(*std::max_element(list.begin(), list.end()) == 8);
}

TEST_CASE("Task 2: random operations behave like std::list", "[task2]") {
    std::mt19937 rng(2026);
    List<int> list;
    std::list<int> model;
    for (int step = 0; step < 3000; ++step) {
        const int v = static_cast<int>(rng() % 100);
        const std::size_t pos = rng() % (model.size() + 1);
        INFO("step " << step);
        if (rng() % 3 != 0 || model.empty()) {
            list.insert(std::next(list.begin(), static_cast<long>(pos)), v);
            model.insert(std::next(model.begin(), static_cast<long>(pos)), v);
        } else {
            const std::size_t at = pos % model.size();
            list.erase(std::next(list.begin(), static_cast<long>(at)));
            model.erase(std::next(model.begin(), static_cast<long>(at)));
        }
        REQUIRE(forwards(list) == std::vector<int>(model.begin(), model.end()));
    }
    checkLinks(list);
}

TEST_CASE("Task 2: no node is leaked", "[task2]") {
    Counted::alive = 0;
    {
        List<Counted> list;
        for (int i = 0; i < 100; ++i) list.pushBack(i);
        for (int i = 0; i < 30; ++i) list.popFront();
        list.erase(std::next(list.begin(), 10));
        REQUIRE(Counted::alive == 69);
        list.clear();
        REQUIRE(Counted::alive == 0);
        REQUIRE(list.empty());
        list.pushBack(1);
        list.pushBack(2);
    }  // destructor
    REQUIRE(Counted::alive == 0);
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: copy", "[task3]") {
    List<std::string> a{"x", "y", "z"};
    List<std::string> b(a);
    REQUIRE(b == a);
    *b.begin() = "changed";
    b.pushBack("w");
    REQUIRE(forwards(a) == std::vector<std::string>{"x", "y", "z"});
    checkLinks(b);

    List<std::string> c{"old"};
    c = a;
    REQUIRE(c == a);
    c = c;
    REQUIRE(c == a);
    checkLinks(c);
}

TEST_CASE("Task 3: a copy that throws halfway leaks nothing", "[task3]") {
    Counted::alive = 0;
    {
        List<Counted> a;
        for (int i = 0; i < 10; ++i) a.pushBack(i);
        Counted::throwOnCopy = 6;  // the 6th copy throws
        REQUIRE_THROWS_AS(List<Counted>(a), std::runtime_error);
        Counted::throwOnCopy = 0;
        REQUIRE(Counted::alive == 10);  // only a's elements: the 5 copies were freed
    }
    REQUIRE(Counted::alive == 0);
}

TEST_CASE("Task 3: move relinks the nodes to the new sentinel", "[task3]") {
    List<int> a{1, 2, 3};
    int* addressOfTwo = &*std::next(a.begin());
    List<int> b(std::move(a));
    REQUIRE(forwards(b) == std::vector<int>{1, 2, 3});
    REQUIRE(&*std::next(b.begin()) == addressOfTwo);  // the same nodes
    checkLinks(b);                                   // incl. --end() on the new sentinel
    REQUIRE(a.empty());                              // NOLINT
    REQUIRE(a.begin() == a.end());
    a.pushBack(7);
    checkLinks(a);

    List<int> c{9};
    c = std::move(b);
    REQUIRE(forwards(c) == std::vector<int>{1, 2, 3});
    checkLinks(c);
    REQUIRE(b.empty());  // NOLINT

    List<int> e;
    List<int> f(std::move(e));  // moving an empty list
    REQUIRE(f.empty());
    REQUIRE(f.begin() == f.end());
    f.pushBack(1);
    checkLinks(f);
}

TEST_CASE("Task 3: swap", "[task3]") {
    List<int> a{1, 2};
    List<int> b{7, 8, 9};
    a.swap(b);
    REQUIRE(forwards(a) == std::vector<int>{7, 8, 9});
    REQUIRE(forwards(b) == std::vector<int>{1, 2});
    checkLinks(a);
    checkLinks(b);
}

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: splice moves nodes without copying", "[task4]") {
    Counted::alive = 0;
    {
        List<Counted> a{1, 2, 5};
        List<Counted> b{3, 4};
        const long before = Counted::alive;
        const Counted* three = &*b.begin();
        a.splice(std::next(a.begin(), 2), b);
        REQUIRE(Counted::alive == before);  // nothing created or destroyed
        REQUIRE(&*std::next(a.begin(), 2) == three);
        std::vector<int> values;
        for (const Counted& c : a) values.push_back(c.value);
        REQUIRE(values == std::vector<int>{1, 2, 3, 4, 5});
        REQUIRE(a.size() == 5);
        REQUIRE(b.empty());
        REQUIRE(b.begin() == b.end());
    }
    REQUIRE(Counted::alive == 0);

    List<int> x{1};
    List<int> empty;
    x.splice(x.end(), empty);
    REQUIRE(forwards(x) == std::vector<int>{1});
    empty.splice(empty.begin(), x);
    REQUIRE(forwards(empty) == std::vector<int>{1});
    checkLinks(empty);
    checkLinks(x);
}

TEST_CASE("Task 4: reverse", "[task4]") {
    List<int> list{1, 2, 3, 4};
    const int* first = &list.front();
    list.reverse();
    REQUIRE(forwards(list) == std::vector<int>{4, 3, 2, 1});
    REQUIRE(&list.back() == first);
    checkLinks(list);
    list.pushBack(0);
    list.pushFront(5);
    REQUIRE(forwards(list) == std::vector<int>{5, 4, 3, 2, 1, 0});
    List<int> empty;
    empty.reverse();
    REQUIRE(empty.begin() == empty.end());
}
