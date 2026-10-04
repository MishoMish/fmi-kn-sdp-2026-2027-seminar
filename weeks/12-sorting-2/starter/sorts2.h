#ifndef SDP_W12_SORTS2_H_
#define SDP_W12_SORTS2_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

// The fast sorts. Same conventions as week 11: random-access iterators,
// compare ONLY with comp(a, b) (the tests count the calls), exchange with
// std::iter_swap, move with std::move.
//
// Why "mergeRanges" and not "merge": with std::vector iterators, an
// unqualified call merge(...) would also find std::merge (argument-
// dependent lookup) and be ambiguous.

// ---------------------------------------------------------------- given

// A uniformly random position in [first, last); fixed seed, so runs repeat.
template <typename RandomIt>
RandomIt randomPivot(RandomIt first, RandomIt last) {
    static std::mt19937 rng(12345);
    const auto n = static_cast<std::size_t>(last - first);
    return first + static_cast<std::ptrdiff_t>(rng() % n);
}

// ---------------------------------------------------------------- Task 1 ★

// Merge two sorted ranges into out; return the end of the output.
// STABLE: on equal elements take from the FIRST range first.
template <typename InIt1, typename InIt2, typename OutIt, typename Compare = std::less<>>
OutIt mergeRanges(InIt1 first1, InIt1 last1, InIt2 first2, InIt2 last2, OutIt out, Compare comp = Compare()) {
    // TODO
    (void)first1;
    (void)last1;
    (void)first2;
    (void)last2;
    (void)comp;
    return out;
}

// Sort [first, last) using buffer[0, last - first) as scratch space:
// sort the two halves recursively, merge them into the buffer, move the
// result back.
template <typename RandomIt, typename BufIt, typename Compare>
void mergeSortWithBuffer(RandomIt first, RandomIt last, BufIt buffer, Compare comp) {
    // TODO
    (void)first;
    (void)last;
    (void)buffer;
    (void)comp;
}

// Stable, Theta(n log n) always, Theta(n) extra memory - allocated ONCE here.
template <typename RandomIt, typename Compare = std::less<>>
void mergeSort(RandomIt first, RandomIt last, Compare comp = Compare()) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    std::vector<T> buffer(static_cast<std::size_t>(last - first));
    mergeSortWithBuffer(first, last, buffer.begin(), comp);
}

// ---------------------------------------------------------------- Task 2 ★★

// Number of pairs i < j with v[i] > v[j], in Theta(n log n): a merge sort
// that counts, while merging, how many elements of the LEFT half are still
// waiting whenever an element of the RIGHT half goes out first.
inline std::uint64_t countInversions(std::vector<int> v) {
    // TODO (a helper function of your own is fine)
    (void)v;
    return 0;
}

// ---------------------------------------------------------------- Task 3 ★★

// Dijkstra's three-way partition ("Dutch national flag") around a COPY of
// the pivot value. Afterwards, with the returned pair (lt, gt):
//   [first, lt)  < pivot      [lt, gt) == pivot      [gt, last)  > pivot
// One pass: lt, i, gt start at first, first, last; look at *i.
template <typename RandomIt, typename T, typename Compare>
std::pair<RandomIt, RandomIt> partition3(RandomIt first, RandomIt last, const T& pivot, Compare comp) {
    // TODO
    (void)pivot;
    (void)comp;
    return {first, last};
}

// Quicksort: pivot = *randomPivot(first, last) (copy it!), partition3, then
// sort the "<" and ">" parts. Recurse into the SMALLER part and loop on the
// larger one: the call stack stays O(log n) even in a bad case.
template <typename RandomIt, typename Compare = std::less<>>
void quickSort(RandomIt first, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)last;
    (void)comp;
}

// ---------------------------------------------------------------- Task 4 ★★

// Quickselect, like std::nth_element: afterwards *nth is the element that
// would be there if [first, last) were sorted, nothing before it is greater
// and nothing after it is less. Partition, then continue ONLY in the part
// that contains nth. Expected Theta(n).
template <typename RandomIt, typename Compare = std::less<>>
void nthElement(RandomIt first, RandomIt nth, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)nth;
    (void)last;
    (void)comp;
}

// ---------------------------------------------------------------- Task 5 ★★ / ★

// Stable counting sort by an integer key in [0, k): key(x) is a size_t.
//   1. count[key] for every element;
//   2. prefix sums: start[key] = where the first element with that key goes;
//   3. walk the input IN ORDER and copy each element to out[start[key]++];
//   4. v = out.
// Theta(n + k) time, Theta(n + k) memory, no comparisons at all.
template <typename T, typename Key>
void countingSortByKey(std::vector<T>& v, std::size_t k, Key key) {
    // TODO
    (void)v;
    (void)k;
    (void)key;
}

// LSD radix sort of 32-bit unsigned ints: four stable counting sorts, by
// byte 0 (the lowest), then 1, 2, 3. Each pass keeps the order of the
// previous passes among equal bytes - that is why it must be stable.
inline void radixSort(std::vector<std::uint32_t>& v) {
    // TODO: countingSortByKey(v, 256, [shift](std::uint32_t x) { ... })
    (void)v;
}

// ---------------------------------------------------------------- Task 6 ★★★

// Bucket sort for doubles in [0, 1): n buckets, x goes to bucket
// floor(x * n); sort each bucket (insertion sort or std::sort - they are
// tiny on uniform input), concatenate. Expected Theta(n) for uniform
// input; still CORRECT for any input in [0, 1).
// Throws std::invalid_argument if a value is outside [0, 1).
inline void bucketSort(std::vector<double>& v) {
    // TODO
    (void)v;
}

#endif  // SDP_W12_SORTS2_H_
