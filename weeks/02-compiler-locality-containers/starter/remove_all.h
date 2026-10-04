#ifndef SDP_W02_REMOVE_ALL_H_
#define SDP_W02_REMOVE_ALL_H_

#include <algorithm>
#include <cstddef>
#include <iterator>

// Task 3: remove every element equal to `value` from a sequence container
// (std::vector, std::deque, std::list, std::string, ...) and return how
// many were removed. The relative order of the remaining elements is kept.
//
// Both functions work for ANY container that has begin(), end() and
// erase(iterator) - they are templates on the container type.

// Erase matching elements one by one with c.erase(it).
// Careful: erase() invalidates `it`. It returns an iterator to the element
// after the erased one - use that.
// For std::vector this is Theta(n^2) in the worst case. Why?
template <typename Container>
std::size_t removeAllNaive(Container& c, const typename Container::value_type& value) {
    // TODO
    (void)c;
    (void)value;
    return 0;
}

// Theta(n) for every container above: compact the kept elements to the
// front in one pass, then erase the tail once. You may use std::remove
// (that is exactly what it does) or write the two-index loop yourself.
template <typename Container>
std::size_t removeAllLinear(Container& c, const typename Container::value_type& value) {
    // TODO
    (void)c;
    (void)value;
    return 0;
}

#endif  // SDP_W02_REMOVE_ALL_H_
