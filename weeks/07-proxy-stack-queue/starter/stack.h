#ifndef SDP_W07_STACK_H_
#define SDP_W07_STACK_H_

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

// Task 1: a stack as an ADAPTOR, like std::stack.
//
// Stack does not store anything itself: it wraps another container and
// exposes only the three stack operations. Everything else the container
// can do (indexing, iterating, inserting in the middle) is hidden.
//
// Container must provide push_back, pop_back, back, size, empty -
// std::vector, std::deque and std::list all do.
template <typename T, typename Container = std::vector<T>>
class Stack {
public:
    using value_type = T;
    using container_type = Container;

    bool empty() const { return c_.empty(); }
    std::size_t size() const { return c_.size(); }

    void push(const T& value) {
        // TODO
        (void)value;
    }

    void push(T&& value) {
        // TODO: move it in
        (void)value;
    }

    // The most recently pushed element. std::out_of_range if empty.
    T& top() {
        // TODO
        throw std::logic_error("TODO: Stack::top");
    }
    const T& top() const {
        // TODO
        throw std::logic_error("TODO: Stack::top");
    }

    // Remove the top. std::out_of_range if empty.
    void pop() {
        // TODO
    }

private:
    Container c_;
};

#endif  // SDP_W07_STACK_H_
