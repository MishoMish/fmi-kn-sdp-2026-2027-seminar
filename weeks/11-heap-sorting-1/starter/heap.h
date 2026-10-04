#ifndef SDP_W11_HEAP_H_
#define SDP_W11_HEAP_H_

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

// A binary heap is an almost complete binary tree stored level by level in
// an array (week 09, Section 4.2):
//
//   children of i: 2i + 1 and 2i + 2        parent of i: (i - 1) / 2
//
// Heap property, as in std::priority_queue: no element is "less" than any
// of its children - with Compare = std::less the LARGEST element is at
// index 0 (a max-heap); with std::greater, the smallest (a min-heap).
//
// The functions work on any random-access range [first, first + n), like
// std::push_heap / std::pop_heap / std::make_heap: first[i] is the i-th
// element. They are used by BinaryHeap below and by heapSort.

// ---------------------------------------------------------------- given

template <typename RandomIt, typename Compare = std::less<>>
bool isHeap(RandomIt first, RandomIt last, Compare comp = Compare()) {
    const auto n = static_cast<std::size_t>(last - first);
    for (std::size_t i = 1; i < n; ++i)
        if (comp(first[(i - 1) / 2], first[i])) return false;  // a parent less than its child
    return true;
}

// ---------------------------------------------------------------- Task 1 ★

// The element at i may be "greater" than its parent: swap it up until it
// is not (or it reaches the root).
template <typename RandomIt, typename Compare>
void siftUp(RandomIt first, std::size_t i, Compare comp) {
    // TODO
    (void)first;
    (void)i;
    (void)comp;
}

// The element at i may be "less" than one of its children (the subtrees
// below it are heaps): swap it with the GREATER child until it is not less
// than either (or it is a leaf). Only [first, first + n) is the heap.
template <typename RandomIt, typename Compare>
void siftDown(RandomIt first, std::size_t i, std::size_t n, Compare comp) {
    // TODO
    (void)first;
    (void)i;
    (void)n;
    (void)comp;
}

// ---------------------------------------------------------------- Task 3 ★★

// Turn an arbitrary range into a heap, Floyd's way: siftDown every
// internal node, from the last one (n/2 - 1) back to the root. Theta(n) -
// the test counts the comparisons: at most 2n.
template <typename RandomIt, typename Compare = std::less<>>
void makeHeap(RandomIt first, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)last;
    (void)comp;
}

// Sort ascending (by comp), in place: makeHeap, then repeatedly swap the
// top (the greatest) with the last element of the heap, shrink the heap by
// one and siftDown the new top. Theta(n log n), O(1) extra memory.
template <typename RandomIt, typename Compare = std::less<>>
void heapSort(RandomIt first, RandomIt last, Compare comp = Compare()) {
    // TODO
    (void)first;
    (void)last;
    (void)comp;
}

// ---------------------------------------------------------------- Task 2 ★

// A priority queue on top of a std::vector, like std::priority_queue:
// top() is the greatest element by Compare.
template <typename T, typename Compare = std::less<T>>
class BinaryHeap {
public:
    BinaryHeap() = default;
    explicit BinaryHeap(Compare comp) : comp_(std::move(comp)) {}

    // Build from existing values in Theta(n) (Task 3's makeHeap).
    explicit BinaryHeap(std::vector<T> values, Compare comp = Compare())
        : data_(std::move(values)), comp_(std::move(comp)) {
        makeHeap(data_.begin(), data_.end(), comp_);
    }

    std::size_t size() const noexcept { return data_.size(); }
    bool empty() const noexcept { return data_.empty(); }

    // The greatest element. Throws std::out_of_range if empty.
    const T& top() const {
        // TODO
        throw std::logic_error("TODO: BinaryHeap::top");
    }

    // Append at the end, then siftUp.
    void push(const T& value) {
        // TODO
        (void)value;
    }

    // Move the last element to the root, drop the last slot, siftDown.
    // Throws std::out_of_range if empty.
    void pop() {
        // TODO
    }

    // For the tests: the underlying array (level order).
    const std::vector<T>& data() const noexcept { return data_; }

private:
    std::vector<T> data_;
    Compare comp_;
};

// ---------------------------------------------------------------- Task 6 ★★

// The k greatest elements of v, greatest first. Keep a heap of the best k
// seen so far, with the WORST of them on top (a min-heap: use a comparator
// that is comp with the arguments swapped). For each element: if the heap
// has fewer than k, push; else if it beats the top, pop and push.
// Theta(n log k) - the test counts comparisons: far fewer than sorting.
template <typename T, typename Compare = std::less<T>>
std::vector<T> topK(const std::vector<T>& v, std::size_t k, Compare comp = Compare()) {
    // TODO: BinaryHeap<T, ...> with a reversed comparator
    (void)v;
    (void)k;
    (void)comp;
    return {};
}

// ---------------------------------------------------------------- Task 6 ★★★

// Merge k sorted (ascending) lists into one sorted list in
// Theta(N log k), N = total length: a min-heap holds one (value, list,
// position) entry per non-empty list; pop the smallest, output it, push
// the next element of the same list.
template <typename T>
std::vector<T> mergeSortedLists(const std::vector<std::vector<T>>& lists) {
    // TODO
    (void)lists;
    return {};
}

#endif  // SDP_W11_HEAP_H_
