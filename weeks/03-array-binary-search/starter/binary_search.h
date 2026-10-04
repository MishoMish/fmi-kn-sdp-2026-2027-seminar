#ifndef SDP_W03_BINARY_SEARCH_H_
#define SDP_W03_BINARY_SEARCH_H_

#include <cstdint>
#include <functional>
#include <iterator>

// Binary search, STL style: works on any random-access range given as a
// pair of iterators [first, last) - std::vector, C arrays, FixedArray, ...
// The range must be sorted according to `less` (default: operator<).
//
// Write every loop around ONE invariant and keep it true:
//
//   [first, lo)  - elements known to be  < value   (too small)
//   [hi, last)   - elements known to be >= value   (not too small)
//   [lo, hi)     - not inspected yet; the loop stops when it is empty

// Task 4: first position whose element is NOT less than value
// (i.e. the first element >= value), or `last` if there is none.
// Same contract as std::lower_bound - but do not call it.
template <typename It, typename T, typename Less = std::less<>>
It lowerBound(It first, It last, const T& value, Less less = {}) {
    // TODO
    (void)value;
    (void)less;
    (void)last;
    return first;
}

// Task 4: first position whose element is GREATER than value, or `last`.
// Same contract as std::upper_bound.
template <typename It, typename T, typename Less = std::less<>>
It upperBound(It first, It last, const T& value, Less less = {}) {
    // TODO
    (void)value;
    (void)less;
    (void)last;
    return first;
}

// Task 4: is `value` in the range? Use lowerBound.
template <typename It, typename T, typename Less = std::less<>>
bool contains(It first, It last, const T& value, Less less = {}) {
    // TODO
    (void)first;
    (void)last;
    (void)value;
    (void)less;
    return false;
}

// Task 5: binary search on the answer.
// `pred` is monotone on [lo, hi): false, false, ..., false, true, ..., true.
// Return the smallest x in [lo, hi) with pred(x) == true, or hi if none.
// Must work for the whole range of std::uint64_t (watch out for lo + hi).
template <typename Pred>
std::uint64_t firstTrue(std::uint64_t lo, std::uint64_t hi, Pred pred) {
    // TODO
    (void)pred;
    (void)lo;
    return hi;
}

// Task 5: floor(sqrt(n)) for every 64-bit n, without floating point.
// Use firstTrue. Careful: mid * mid overflows for large mid.
std::uint64_t integerSqrt(std::uint64_t n);

#endif  // SDP_W03_BINARY_SEARCH_H_
