#ifndef SDP_W07_TWO_STACK_QUEUE_H_
#define SDP_W07_TWO_STACK_QUEUE_H_

#include <cstddef>
#include <stdexcept>

#include "stack.h"

// Task 3: a FIFO queue built from two LIFO stacks.
//
//   push  -> onto in_
//   front / pop -> from out_; if out_ is empty, first move EVERYTHING
//                  from in_ to out_ (which reverses the order).
//
// One pop may cost Theta(n), but every element is moved from in_ to out_
// at most once: amortized Theta(1) per operation (week 04!).
template <typename T>
class TwoStackQueue {
public:
    bool empty() const { return in_.empty() && out_.empty(); }
    std::size_t size() const { return in_.size() + out_.size(); }

    void push(const T& value) {
        // TODO
        (void)value;
    }

    // std::out_of_range if empty.
    T& front() {
        // TODO
        throw std::logic_error("TODO: TwoStackQueue::front");
    }

    // std::out_of_range if empty.
    void pop() {
        // TODO
    }

    // How many single-element transfers from in_ to out_ so far (for the tests).
    long transfers() const { return transfers_; }

private:
    void refill() {
        // TODO: only if out_ is empty - move every element of in_ onto out_
    }

    Stack<T> in_;
    Stack<T> out_;
    long transfers_ = 0;
};

#endif  // SDP_W07_TWO_STACK_QUEUE_H_
