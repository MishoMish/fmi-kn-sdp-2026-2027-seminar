#include <catch_amalgamated.hpp>

#include <algorithm>
#include <array>
#include <deque>
#include <functional>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "counting.h"
#include "sorts.h"

namespace {

using V = std::vector<int>;
using Pair = std::pair<int, int>;  // (key, original position)

V randomInts(std::size_t n, unsigned seed, int maxValue = 1'000'000) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> d(0, maxValue);
    V v(n);
    for (int& x : v) x = d(rng);
    return v;
}

std::size_t inversions(const V& v) {
    std::size_t inv = 0;
    for (std::size_t i = 0; i < v.size(); ++i)
        for (std::size_t j = i + 1; j < v.size(); ++j) inv += v[j] < v[i];
    return inv;
}

// Every sort must pass these.
template <typename Sort>
void checkSorts(Sort sort) {
    for (std::size_t n : {0u, 1u, 2u, 3u, 10u, 100u, 1000u}) {
        for (int kind = 0; kind < 4; ++kind) {
            V a = randomInts(n, static_cast<unsigned>(n * 4 + kind), kind == 3 ? 5 : 1'000'000);
            if (kind == 1) std::sort(a.begin(), a.end());
            if (kind == 2) std::sort(a.begin(), a.end(), std::greater<>());
            V expected = a;
            std::sort(expected.begin(), expected.end());
            sort(a.begin(), a.end(), std::less<>());
            INFO("n = " << n << ", kind = " << kind);  // random, sorted, reversed, few values
            REQUIRE(a == expected);
        }
    }
    std::vector<std::string> words{"tree", "heap", "graph", "array", "list"};
    sort(words.begin(), words.end(), std::greater<>());
    REQUIRE(words == std::vector<std::string>{"tree", "list", "heap", "graph", "array"});

    std::deque<int> d{3, 1, 2};  // any random-access iterator
    sort(d.begin(), d.end(), std::less<>());
    REQUIRE(d == std::deque<int>{1, 2, 3});

    std::array<int, 4> arr{4, 3, 2, 1};
    sort(arr.begin(), arr.end(), std::less<>());
    REQUIRE(arr == std::array<int, 4>{1, 2, 3, 4});
}

// Stable sorts keep equal keys in their original order.
template <typename Sort>
void checkStable(Sort sort) {
    std::mt19937 rng(77);
    std::vector<Pair> a(500);
    for (std::size_t i = 0; i < a.size(); ++i) a[i] = {static_cast<int>(rng() % 10), static_cast<int>(i)};
    std::vector<Pair> expected = a;
    std::stable_sort(expected.begin(), expected.end(), [](const Pair& x, const Pair& y) { return x.first < y.first; });
    sort(a.begin(), a.end(), [](const Pair& x, const Pair& y) { return x.first < y.first; });
    REQUIRE(a == expected);
}

#define SORT_FN(f) [](auto first, auto last, auto comp) { f(first, last, comp); }

}  // namespace

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: selectionSort sorts", "[task4]") { checkSorts(SORT_FN(selectionSort)); }

TEST_CASE("Task 4: selectionSort - always n(n-1)/2 comparisons, O(n) writes", "[task4]") {
    for (int kind = 0; kind < 2; ++kind) {
        V a = randomInts(500, 1);
        if (kind == 1) std::sort(a.begin(), a.end());
        std::size_t count = 0;
        selectionSort(a.begin(), a.end(), CountingLess{&count});
        REQUIRE(count == 500u * 499u / 2u);  // even when already sorted
    }
    std::vector<Tracked> t;
    for (int x : randomInts(500, 2)) t.emplace_back(x);
    Tracked::writes = 0;
    selectionSort(t.begin(), t.end());
    REQUIRE(Tracked::writes <= 3 * 499);  // at most n - 1 swaps, 3 moves each
    REQUIRE(std::is_sorted(t.begin(), t.end()));
}

TEST_CASE("Task 4: bubbleSort sorts, is stable, stops early", "[task4]") {
    checkSorts(SORT_FN(bubbleSort));
    checkStable(SORT_FN(bubbleSort));
    V sorted(1000);
    std::iota(sorted.begin(), sorted.end(), 0);
    std::size_t count = 0;
    bubbleSort(sorted.begin(), sorted.end(), CountingLess{&count});
    REQUIRE(count == 999);  // one pass, no swaps, done
}

TEST_CASE("Task 4: insertionSort sorts and is stable", "[task4]") {
    checkSorts(SORT_FN(insertionSort));
    checkStable(SORT_FN(insertionSort));
}

TEST_CASE("Task 4: insertionSort is Theta(n + inversions)", "[task4]") {
    V sorted(1000);
    std::iota(sorted.begin(), sorted.end(), 0);
    std::size_t count = 0;
    insertionSort(sorted.begin(), sorted.end(), CountingLess{&count});
    REQUIRE(count == 999);

    V nearly = sorted;  // 10 random neighbour swaps: at most 10 inversions
    std::mt19937 rng(4);
    for (int k = 0; k < 10; ++k) {
        const std::size_t i = rng() % 999;
        std::swap(nearly[i], nearly[i + 1]);
    }
    const std::size_t inv = inversions(nearly);
    count = 0;
    insertionSort(nearly.begin(), nearly.end(), CountingLess{&count});
    REQUIRE(std::is_sorted(nearly.begin(), nearly.end()));
    REQUIRE(count <= 999 + inv);

    V random = randomInts(1000, 5);
    const std::size_t invRandom = inversions(random);
    count = 0;
    insertionSort(random.begin(), random.end(), CountingLess{&count});
    REQUIRE(count <= 999 + invRandom);
    REQUIRE(count >= invRandom);
}

TEST_CASE("Task 4: insertionSort shifts instead of swapping", "[task4]") {
    std::vector<Tracked> t;
    for (int i = 300; i > 0; --i) t.emplace_back(i);  // reversed: 300*299/2 inversions
    Tracked::writes = 0;
    insertionSort(t.begin(), t.end());
    REQUIRE(std::is_sorted(t.begin(), t.end()));
    // one write per shift + 2 per element; swapping would need 3 per inversion
    REQUIRE(Tracked::writes <= 300u * 299u / 2u + 2u * 300u);
}

// ---------------------------------------------------------------- Task 5

TEST_CASE("Task 5: shakerSort sorts and is stable", "[task5]") {
    checkSorts(SORT_FN(shakerSort));
    checkStable(SORT_FN(shakerSort));
}

TEST_CASE("Task 5: shakerSort moves turtles in one backward pass", "[task5]") {
    // 1, 2, ..., n-1, then 0: the 0 is a "turtle" - bubble sort moves it one
    // step per pass (n - 1 passes); a backward pass carries it all the way.
    constexpr int n = 1000;
    V a(n);
    std::iota(a.begin(), a.end(), 1);
    a.back() = 0;
    V b = a;
    std::size_t shaker = 0;
    shakerSort(a.begin(), a.end(), CountingLess{&shaker});
    REQUIRE(std::is_sorted(a.begin(), a.end()));
    REQUIRE(shaker <= 3 * n);
    std::size_t bubble = 0;
    bubbleSort(b.begin(), b.end(), CountingLess{&bubble});
    INFO("shaker: " << shaker << ", bubble: " << bubble);
    REQUIRE(bubble > 100 * shaker);
}

TEST_CASE("Task 5: shellSort sorts", "[task5]") { checkSorts(SORT_FN(shellSort)); }

TEST_CASE("Task 5: shellSort is far below quadratic", "[task5]") {
    const V a = randomInts(100'000, 9);
    V b = a;
    std::size_t count = 0;
    shellSort(b.begin(), b.end(), CountingLess{&count});
    REQUIRE(std::is_sorted(b.begin(), b.end()));
    INFO(count << " comparisons");
    REQUIRE(count < 5'000'000);  // insertion sort: ~2 500 000 000

    V sorted(100'000);
    std::iota(sorted.begin(), sorted.end(), 0);
    count = 0;
    shellSort(sorted.begin(), sorted.end(), CountingLess{&count});
    REQUIRE(count < 2'000'000);  // one comparison per element per gap
}
