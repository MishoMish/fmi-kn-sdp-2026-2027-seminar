#ifndef SDP_W07_APPLICATIONS_H_
#define SDP_W07_APPLICATIONS_H_

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "stack.h"

// Task 5: classic uses of a stack and of a double-ended queue.

// ★ Are the brackets ( ) [ ] { } in s balanced and properly nested?
// Other characters are ignored. "([]{})" -> true, "([)]" -> false.
bool balancedBrackets(const std::string& s);

// ★★ A stack that also answers "what is the smallest element?" in Theta(1).
// Hint: a second stack that remembers the minimum at every height.
template <typename T>
class MinStack {
public:
    bool empty() const { return values_.empty(); }
    std::size_t size() const { return values_.size(); }

    void push(const T& value) {
        // TODO
        (void)value;
    }

    void pop() {
        // TODO (std::out_of_range if empty)
    }

    const T& top() const {
        // TODO
        throw std::logic_error("TODO: MinStack::top");
    }

    // Smallest element currently in the stack. Theta(1). std::out_of_range if empty.
    const T& min() const {
        // TODO
        throw std::logic_error("TODO: MinStack::min");
    }

private:
    Stack<T> values_;
    Stack<T> mins_;
};

// ★★★ For every window of k consecutive elements, its maximum.
// [1, 3, -1, -3, 5, 3, 6, 7], k = 3  ->  [3, 3, 5, 5, 6, 7].
// Theta(n) total with a double-ended queue of INDICES whose values are
// decreasing from front to back (a "monotonic deque").
// std::invalid_argument if k == 0 or k > a.size().
std::vector<int> slidingWindowMax(const std::vector<int>& a, std::size_t k);

#endif  // SDP_W07_APPLICATIONS_H_
