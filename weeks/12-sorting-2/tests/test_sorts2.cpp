#include <catch_amalgamated.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <deque>
#include <functional>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "counting.h"
#include "sorts2.h"

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

double nlog2n(std::size_t n) { return static_cast<double>(n) * std::log2(static_cast<double>(n)); }

template <typename Sort>
void checkSorts(Sort sort) {
    for (std::size_t n : {0u, 1u, 2u, 3u, 10u, 100u, 1000u, 20'000u}) {
        for (int kind = 0; kind < 5; ++kind) {
            V a = randomInts(n, static_cast<unsigned>(n * 5 + kind), kind == 3 ? 5 : 1'000'000);
            if (kind == 1) std::sort(a.begin(), a.end());
            if (kind == 2) std::sort(a.begin(), a.end(), std::greater<>());
            if (kind == 4) std::fill(a.begin(), a.end(), 7);
            V expected = a;
            std::sort(expected.begin(), expected.end());
            sort(a.begin(), a.end(), std::less<>());
            INFO("n = " << n << ", kind = " << kind);  // random, sorted, reversed, few values, all equal
            REQUIRE(a == expected);
        }
    }
    std::vector<std::string> words{"tree", "heap", "graph", "array", "list"};
    sort(words.begin(), words.end(), std::greater<>());
    REQUIRE(words == std::vector<std::string>{"tree", "list", "heap", "graph", "array"});
    std::deque<int> d{3, 1, 2};
    sort(d.begin(), d.end(), std::less<>());
    REQUIRE(d == std::deque<int>{1, 2, 3});
}

std::uint64_t bruteInversions(const V& v) {
    std::uint64_t inv = 0;
    for (std::size_t i = 0; i < v.size(); ++i)
        for (std::size_t j = i + 1; j < v.size(); ++j) inv += v[j] < v[i];
    return inv;
}

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: mergeRanges", "[task1]") {
    V a{1, 4, 7, 9}, b{2, 3, 8}, out(7);
    REQUIRE(mergeRanges(a.begin(), a.end(), b.begin(), b.end(), out.begin()) == out.end());
    REQUIRE(out == V{1, 2, 3, 4, 7, 8, 9});

    V empty, single{5}, out1(1);
    mergeRanges(empty.begin(), empty.end(), single.begin(), single.end(), out1.begin());
    REQUIRE(out1 == V{5});

    std::size_t count = 0;  // at most n - 1 comparisons
    V x(1000), y(1000), z(2000);
    std::iota(x.begin(), x.end(), 0);
    std::iota(y.begin(), y.end(), 500);
    mergeRanges(x.begin(), x.end(), y.begin(), y.end(), z.begin(), CountingLess{&count});
    REQUIRE(std::is_sorted(z.begin(), z.end()));
    REQUIRE(count <= 1999);
}

TEST_CASE("Task 1: mergeRanges is stable - ties come from the first range", "[task1]") {
    std::vector<Pair> a{{1, 0}, {2, 0}, {2, 1}}, b{{1, 1}, {2, 2}}, out(5);
    mergeRanges(a.begin(), a.end(), b.begin(), b.end(), out.begin(),
                [](const Pair& p, const Pair& q) { return p.first < q.first; });
    REQUIRE(out == std::vector<Pair>{{1, 0}, {1, 1}, {2, 0}, {2, 1}, {2, 2}});
}

TEST_CASE("Task 1: mergeSort sorts", "[task1]") {
    checkSorts([](auto f, auto l, auto c) { mergeSort(f, l, c); });
}

TEST_CASE("Task 1: mergeSort is stable and n log n", "[task1]") {
    std::mt19937 rng(1);
    std::vector<Pair> a(5000);
    for (std::size_t i = 0; i < a.size(); ++i) a[i] = {static_cast<int>(rng() % 20), static_cast<int>(i)};
    auto expected = a;
    auto byKey = [](const Pair& p, const Pair& q) { return p.first < q.first; };
    std::stable_sort(expected.begin(), expected.end(), byKey);
    mergeSort(a.begin(), a.end(), byKey);
    REQUIRE(a == expected);

    for (int kind = 0; kind < 2; ++kind) {
        V v = randomInts(1 << 16, 3);
        if (kind == 1) std::sort(v.begin(), v.end(), std::greater<>());
        std::size_t count = 0;
        mergeSort(v.begin(), v.end(), CountingLess{&count});
        REQUIRE(std::is_sorted(v.begin(), v.end()));
        REQUIRE(count <= static_cast<std::size_t>(nlog2n(v.size())));  // at most n log2 n
    }
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: countInversions", "[task2]") {
    REQUIRE(countInversions({}) == 0);
    REQUIRE(countInversions({1, 2, 3}) == 0);
    REQUIRE(countInversions({3, 2, 1}) == 3);
    REQUIRE(countInversions({3, 1, 4, 1, 5, 9, 2, 6}) == 8);
    REQUIRE(countInversions({2, 2, 2}) == 0);  // equal elements are not inversions
    for (unsigned seed = 0; seed < 20; ++seed) {
        const V v = randomInts(300 + seed * 37, seed, seed < 10 ? 10 : 1'000'000);
        REQUIRE(countInversions(v) == bruteInversions(v));
    }
}

TEST_CASE("Task 2: countInversions on 200 000 elements (needs 64 bits)", "[task2]") {
    V v(200'000);
    std::iota(v.rbegin(), v.rend(), 0);  // descending: every pair is an inversion
    REQUIRE(countInversions(v) == 200'000ull * 199'999ull / 2ull);  // ~2 * 10^10 > 2^32
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: partition3", "[task3]") {
    V a{5, 1, 9, 5, 3, 5, 8, 2, 5};
    const auto [lt, gt] = partition3(a.begin(), a.end(), 5, std::less<>());
    REQUIRE(lt - a.begin() == 3);
    REQUIRE(gt - a.begin() == 7);
    REQUIRE(std::all_of(a.begin(), lt, [](int x) { return x < 5; }));
    REQUIRE(std::all_of(lt, gt, [](int x) { return x == 5; }));
    REQUIRE(std::all_of(gt, a.end(), [](int x) { return x > 5; }));

    V b{1, 2, 3};
    auto r = partition3(b.begin(), b.end(), 10, std::less<>());  // all less
    REQUIRE(r.first == b.end());
    REQUIRE(r.second == b.end());

    std::size_t count = 0;  // one pass: at most 2 comparisons per element
    V c = randomInts(10'000, 4, 100);
    partition3(c.begin(), c.end(), 50, CountingLess{&count});
    REQUIRE(count <= 2 * c.size());
}

TEST_CASE("Task 3: quickSort sorts", "[task3]") {
    checkSorts([](auto f, auto l, auto c) { quickSort(f, l, c); });
}

TEST_CASE("Task 3: quickSort is n log n on sorted and on all-equal input", "[task3]") {
    constexpr std::size_t n = 1 << 16;
    V almost(n);  // sorted, then the largest moved to the front: n-1, 0, 1, ..., n-2
    std::iota(almost.begin(), almost.end(), 0);
    std::rotate(almost.begin(), almost.end() - 1, almost.end());
    std::size_t count = 0;
    quickSort(almost.begin(), almost.end(), CountingLess{&count});
    REQUIRE(std::is_sorted(almost.begin(), almost.end()));
    INFO("almost sorted input: " << count << " comparisons");
    REQUIRE(count <= static_cast<std::size_t>(4 * nlog2n(n)));  // first-element pivot: ~n^2 / 2

    V equal(n, 42);
    count = 0;
    quickSort(equal.begin(), equal.end(), CountingLess{&count});
    INFO("all equal: " << count << " comparisons");
    REQUIRE(count >= n - 1);  // any correct sort must look at every element
    REQUIRE(count <= 2 * n);  // one partition pass, nothing left on either side
}

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: nthElement puts the right element in place", "[task4]") {
    for (unsigned seed = 0; seed < 30; ++seed) {
        V a = randomInts(1 + seed * 31, seed, seed % 2 ? 20 : 1'000'000);
        V sorted = a;
        std::sort(sorted.begin(), sorted.end());
        const std::size_t k = (seed * 7) % a.size();
        nthElement(a.begin(), a.begin() + static_cast<std::ptrdiff_t>(k), a.end());
        INFO("n = " << a.size() << ", k = " << k);
        REQUIRE(a[k] == sorted[k]);
        for (std::size_t i = 0; i < k; ++i) REQUIRE(a[i] <= a[k]);
        for (std::size_t i = k + 1; i < a.size(); ++i) REQUIRE(a[i] >= a[k]);
    }
}

TEST_CASE("Task 4: nthElement is linear on average", "[task4]") {
    V a = randomInts(100'000, 8);
    V sorted = a;
    std::sort(sorted.begin(), sorted.end());
    std::size_t count = 0;
    const auto mid = a.begin() + 50'000;
    nthElement(a.begin(), mid, a.end(), CountingLess{&count});
    REQUIRE(*mid == sorted[50'000]);
    INFO(count << " comparisons");
    REQUIRE(count < 15 * a.size());  // sorting would be ~1 700 000; this is ~700 000
}

// ---------------------------------------------------------------- Task 5

TEST_CASE("Task 5: countingSortByKey is stable", "[task5]") {
    std::vector<Pair> a{{3, 0}, {1, 1}, {3, 2}, {0, 3}, {1, 4}, {3, 5}};
    countingSortByKey(a, 4, [](const Pair& p) { return static_cast<std::size_t>(p.first); });
    REQUIRE(a == std::vector<Pair>{{0, 3}, {1, 1}, {1, 4}, {3, 0}, {3, 2}, {3, 5}});

    std::vector<std::string> words{"pear", "fig", "apple", "kiwi", "plum", "date", "banana"};
    countingSortByKey(words, 10, [](const std::string& w) { return w.size(); });  // by length
    REQUIRE(words == std::vector<std::string>{"fig", "pear", "kiwi", "plum", "date", "apple", "banana"});

    std::vector<int> empty;
    countingSortByKey(empty, 5, [](int x) { return static_cast<std::size_t>(x); });
    REQUIRE(empty.empty());
}

TEST_CASE("Task 5: countingSortByKey sorts small ints", "[task5]") {
    V a = randomInts(100'000, 9, 999);
    V expected = a;
    std::sort(expected.begin(), expected.end());
    countingSortByKey(a, 1000, [](int x) { return static_cast<std::size_t>(x); });
    REQUIRE(a == expected);
}

TEST_CASE("Task 5: radixSort", "[task5]") {
    std::mt19937 rng(10);
    std::vector<std::uint32_t> a(200'000);
    for (auto& x : a) x = static_cast<std::uint32_t>(rng());
    a[0] = 0;
    a[1] = 0xFFFFFFFFu;
    auto expected = a;
    std::sort(expected.begin(), expected.end());
    radixSort(a);
    REQUIRE(a == expected);

    // only the HIGHEST byte differs: the last pass decides everything
    std::vector<std::uint32_t> b{0x03000000u, 0x01000000u, 0x02000000u, 0x01000001u};
    radixSort(b);
    REQUIRE(b == std::vector<std::uint32_t>{0x01000000u, 0x01000001u, 0x02000000u, 0x03000000u});

    std::vector<std::uint32_t> c;
    radixSort(c);
    REQUIRE(c.empty());
}

// ---------------------------------------------------------------- Task 6

TEST_CASE("Task 6: bucketSort", "[task6]") {
    std::mt19937 rng(11);
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    std::vector<double> a(100'000);
    for (double& x : a) x = uniform(rng);
    auto expected = a;
    std::sort(expected.begin(), expected.end());
    bucketSort(a);
    REQUIRE(a == expected);

    // far from uniform: everything in one bucket - slow, but still correct
    std::vector<double> b(2000);
    for (double& x : b) x = uniform(rng) / 10'000.0;
    expected = b;
    std::sort(expected.begin(), expected.end());
    bucketSort(b);
    REQUIRE(b == expected);

    std::vector<double> edge{0.0, std::nextafter(1.0, 0.0), 0.5};  // the largest double below 1
    bucketSort(edge);
    REQUIRE(edge == std::vector<double>{0.0, 0.5, std::nextafter(1.0, 0.0)});

    std::vector<double> bad{0.5, 1.0};
    REQUIRE_THROWS_AS(bucketSort(bad), std::invalid_argument);
    std::vector<double> negative{-0.1};
    REQUIRE_THROWS_AS(bucketSort(negative), std::invalid_argument);
}
