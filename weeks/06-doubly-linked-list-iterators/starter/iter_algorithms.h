#ifndef SDP_W06_ITER_ALGORITHMS_H_
#define SDP_W06_ITER_ALGORITHMS_H_

#include <iterator>
#include <type_traits>
#include <utility>

// Task 5: algorithms written ONLY against iterators. Each one works for
// every container whose iterators are strong enough: your List, std::vector,
// C arrays, std::forward_list, ...

// ★ First position in [first, last) equal to value, or last.
// Needs only ++, * and != (an input iterator).
template <typename It, typename T>
It findValue(It first, It last, const T& value) {
    // TODO
    (void)value;
    (void)last;
    return first;
}

// ★★ Reverse [first, last) in place by swapping values from both ends.
// Needs -- (a bidirectional iterator). Careful with even and odd lengths:
// the two iterators must never cross. Use std::iter_swap(a, b).
template <typename BidirIt>
void reverseRange(BidirIt first, BidirIt last) {
    // TODO
    (void)first;
    (void)last;
}

// ★★ Does [first, last) read the same forwards and backwards?
template <typename BidirIt>
bool isPalindrome(BidirIt first, BidirIt last) {
    // TODO
    (void)first;
    (void)last;
    return false;
}

// ★★★ Number of steps from first to last.
//   random-access iterators (vector, array): Theta(1) - just last - first
//   all others (List, forward_list):        Theta(n) - count the ++ steps
// Pick the version at compile time from
//   typename std::iterator_traits<It>::iterator_category
// with if constexpr and std::is_base_of_v<std::random_access_iterator_tag, ...>.
template <typename It>
typename std::iterator_traits<It>::difference_type distanceBetween(It first, It last) {
    // TODO
    (void)first;
    (void)last;
    return 0;
}

#endif  // SDP_W06_ITER_ALGORITHMS_H_
