#ifndef SDP_W11_SORTS_H_
#define SDP_W11_SORTS_H_

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <utility>

// The simple sorts, written like std::sort: a random-access range
// [first, last) and a comparator. Use first[i] (or iterators) and only
// comp(a, b) to compare - never < directly: the tests pass a comparator
// that COUNTS comparisons, and one that compares only a part of the
// element (to check stability).
//
// Use std::swap or std::iter_swap to exchange elements, and
// std::move to shift them (insertion sort and Shell sort shift instead of
// swapping: one assignment per step instead of three).

// ---------------------------------------------------------------- Task 4 ★

// Selection sort: for i = 0, 1, ...: find the smallest in [i, n) and swap
// it into position i. Always n(n-1)/2 comparisons, at most n - 1 swaps.
template <typename RandomIt, typename Compare = std::less<>>
void selectionSort(RandomIt first, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)last;
    (void)comp;
}

// Bubble sort: pass after pass, swap neighbours that are out of order; the
// greatest "bubbles" to the end. Stop as soon as a pass makes no swap - on
// sorted input that is a single pass of n - 1 comparisons. Stable: swap
// only when comp(right, left), never on equal elements.
template <typename RandomIt, typename Compare = std::less<>>
void bubbleSort(RandomIt first, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)last;
    (void)comp;
}

// Insertion sort: for i = 1, 2, ...: take first[i] out and shift the
// greater elements of the sorted prefix one step right, then put it into
// the gap. Theta(n + inversions); stable.
template <typename RandomIt, typename Compare = std::less<>>
void insertionSort(RandomIt first, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)last;
    (void)comp;
}

// ---------------------------------------------------------------- Task 5 ★★

// Shaker (cocktail) sort: bubble sort that alternates directions - left to
// right (the greatest goes to the end), then right to left (the smallest
// goes to the front). Keep both ends of the unsorted part; stop when a
// pass makes no swap. Stable.
template <typename RandomIt, typename Compare = std::less<>>
void shakerSort(RandomIt first, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)last;
    (void)comp;
}

// Shell sort: insertion sort on elements gap apart, for decreasing gaps,
// ending with gap 1 (a plain insertion sort, on an almost sorted array).
// Gaps: Ciura's 1, 4, 10, 23, 57, 132, 301, 701, then each next is
// about 2.25 times the previous. Start with the largest gap < n.
template <typename RandomIt, typename Compare = std::less<>>
void shellSort(RandomIt first, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)last;
    (void)comp;
}

#endif  // SDP_W11_SORTS_H_
