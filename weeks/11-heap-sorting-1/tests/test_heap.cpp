#include <catch_amalgamated.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <vector>

#include "counting.h"
#include "heap.h"

namespace {

using V = std::vector<int>;

V randomInts(std::size_t n, unsigned seed, int maxValue = 1'000'000) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> d(0, maxValue);
    V v(n);
    for (int& x : v) x = d(rng);
    return v;
}

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: siftUp moves a new element up to its place", "[task1]") {
    //        9
    //      /   \        (heap; 10 appended as the left child of 5)
    //     5     8
    //    /
    //   10
    V a{9, 5, 8, 10};
    siftUp(a.begin(), 3, std::less<>());
    REQUIRE(a == V{10, 9, 8, 5});

    V b{9, 5, 8, 1};  // 1 is fine where it is
    siftUp(b.begin(), 3, std::less<>());
    REQUIRE(b == V{9, 5, 8, 1});
}

TEST_CASE("Task 1: siftDown swaps with the GREATER child", "[task1]") {
    V a{1, 9, 8, 5, 4, 7, 6};
    siftDown(a.begin(), 0, a.size(), std::less<>());
    REQUIRE(a == V{9, 5, 8, 1, 4, 7, 6});
    REQUIRE(isHeap(a.begin(), a.end()));

    V b{1, 9, 8};  // only [0, 1) is "the heap": nothing below the root
    siftDown(b.begin(), 0, 1, std::less<>());
    REQUIRE(b == V{1, 9, 8});

    V c{3, 5};  // a single (left) child
    siftDown(c.begin(), 0, 2, std::less<>());
    REQUIRE(c == V{5, 3});
}

TEST_CASE("Task 1: a min-heap with std::greater", "[task1]") {
    V a{2, 5, 3, 1};
    siftUp(a.begin(), 3, std::greater<>());
    REQUIRE(a == V{1, 2, 3, 5});
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: push, top, pop", "[task2]") {
    BinaryHeap<int> h;
    REQUIRE(h.empty());
    for (int x : {5, 1, 9, 3, 7}) {
        h.push(x);
        REQUIRE(isHeap(h.data().begin(), h.data().end()));
    }
    REQUIRE(h.size() == 5);
    V out;
    while (!h.empty()) {
        out.push_back(h.top());
        h.pop();
        REQUIRE(isHeap(h.data().begin(), h.data().end()));
    }
    REQUIRE(out == V{9, 7, 5, 3, 1});
    REQUIRE_THROWS_AS(h.top(), std::out_of_range);
    REQUIRE_THROWS_AS(h.pop(), std::out_of_range);
}

TEST_CASE("Task 2: random operations against std::priority_queue", "[task2]") {
    std::mt19937 rng(11);
    BinaryHeap<int> h;
    std::priority_queue<int> ref;
    for (int step = 0; step < 20'000; ++step) {
        if (ref.empty() || rng() % 3 != 0) {
            const int x = static_cast<int>(rng() % 1000);
            h.push(x);
            ref.push(x);
        } else {
            REQUIRE(h.top() == ref.top());
            h.pop();
            ref.pop();
        }
        REQUIRE(h.size() == ref.size());
    }
    REQUIRE(isHeap(h.data().begin(), h.data().end()));
}

TEST_CASE("Task 2: a min-heap of strings", "[task2]") {
    BinaryHeap<std::string, std::greater<std::string>> h;
    for (const char* w : {"tree", "heap", "graph", "array", "list"}) h.push(w);
    REQUIRE(h.top() == "array");
    h.pop();
    REQUIRE(h.top() == "graph");
}

TEST_CASE("Task 2: push and pop are logarithmic", "[task2]") {
    std::size_t count = 0;
    BinaryHeap<int, CountingLess> h(CountingLess{&count});
    constexpr int n = 1 << 14;
    for (int i = 0; i < n; ++i) h.push(i);  // each new one is the greatest: sifts to the root
    REQUIRE(h.size() == static_cast<std::size_t>(n));
    REQUIRE(h.top() == n - 1);
    REQUIRE(count <= static_cast<std::size_t>(n) * 14);
    count = 0;
    while (!h.empty()) h.pop();
    REQUIRE(count <= static_cast<std::size_t>(n) * 2 * 14);  // two comparisons per level
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: makeHeap builds a heap", "[task3]") {
    for (std::size_t n : {0u, 1u, 2u, 3u, 10u, 1000u}) {
        V a = randomInts(n, static_cast<unsigned>(n));
        V copy = a;
        makeHeap(a.begin(), a.end());
        INFO("n = " << n);
        REQUIRE(isHeap(a.begin(), a.end()));
        std::sort(a.begin(), a.end());
        std::sort(copy.begin(), copy.end());
        REQUIRE(a == copy);  // the same elements
    }
}

TEST_CASE("Task 3: makeHeap is linear - at most 2n comparisons", "[task3]") {
    constexpr std::size_t n = 1 << 16;
    for (int order = 0; order < 3; ++order) {
        V a(n);
        std::iota(a.begin(), a.end(), 0);  // ascending: the worst case for repeated push
        if (order == 1) std::reverse(a.begin(), a.end());
        if (order == 2) a = randomInts(n, 3);
        std::size_t count = 0;
        makeHeap(a.begin(), a.end(), CountingLess{&count});
        INFO("order " << order << ": " << count << " comparisons");
        REQUIRE(isHeap(a.begin(), a.end()));
        REQUIRE(count <= 2 * n);  // n pushes would need ~n log2 n = 16n here
    }
}

TEST_CASE("Task 3: heapSort", "[task3]") {
    for (std::size_t n : {0u, 1u, 2u, 7u, 1000u, 50'000u}) {
        V a = randomInts(n, 7 + static_cast<unsigned>(n), 100);  // many duplicates
        V expected = a;
        std::sort(expected.begin(), expected.end());
        std::size_t count = 0;
        heapSort(a.begin(), a.end(), CountingLess{&count});
        INFO("n = " << n);
        REQUIRE(a == expected);
        if (n > 1) REQUIRE(count <= 2 * n * static_cast<std::size_t>(std::log2(n)) + 2 * n);
    }
    std::vector<std::string> words{"tree", "heap", "graph", "array", "list"};
    heapSort(words.begin(), words.end(), std::greater<>());
    REQUIRE(words == std::vector<std::string>{"tree", "list", "heap", "graph", "array"});

    int raw[] = {3, 1, 2};  // plain pointers are random-access iterators too
    heapSort(std::begin(raw), std::end(raw));
    REQUIRE(raw[0] == 1);
    REQUIRE(raw[2] == 3);
}

TEST_CASE("Task 3: BinaryHeap from a vector", "[task3]") {
    BinaryHeap<int> h(V{4, 8, 1, 9, 2});
    REQUIRE(isHeap(h.data().begin(), h.data().end()));
    REQUIRE(h.top() == 9);
}

// ---------------------------------------------------------------- Task 6

TEST_CASE("Task 6: topK", "[task6]") {
    REQUIRE(topK(V{5, 1, 9, 3, 7, 9}, 3) == V{9, 9, 7});
    REQUIRE(topK(V{5, 1}, 5) == V{5, 1});  // fewer than k
    REQUIRE(topK(V{5, 1}, 0).empty());
    REQUIRE(topK(V{}, 3).empty());
    REQUIRE(topK(V{5, 1, 9, 3}, 2, std::greater<int>()) == V{1, 3});  // the 2 "greatest" by greater
}

TEST_CASE("Task 6: topK is n log k, not n log n", "[task6]") {
    const V v = randomInts(100'000, 6);
    V expected = v;
    std::sort(expected.begin(), expected.end(), std::greater<>());
    expected.resize(10);
    std::size_t count = 0;
    REQUIRE(topK(v, 10, CountingLess{&count}) == expected);
    INFO(count << " comparisons");
    REQUIRE(count < 2 * v.size());  // sorting 100 000 would be ~1 700 000
}

TEST_CASE("Task 6: mergeSortedLists", "[task6]") {
    REQUIRE(mergeSortedLists<int>({{1, 4, 7}, {2, 5, 8}, {3, 6, 9}}) == V{1, 2, 3, 4, 5, 6, 7, 8, 9});
    REQUIRE(mergeSortedLists<int>({{}, {1}, {}, {0, 0, 2}}) == V{0, 0, 1, 2});
    REQUIRE(mergeSortedLists<int>({}).empty());

    std::mt19937 rng(12);
    std::vector<V> lists(1000);
    V all;
    for (V& l : lists) {
        l = randomInts(rng() % 50, static_cast<unsigned>(rng()));
        std::sort(l.begin(), l.end());
        all.insert(all.end(), l.begin(), l.end());
    }
    std::sort(all.begin(), all.end());
    REQUIRE(mergeSortedLists(lists) == all);
}
