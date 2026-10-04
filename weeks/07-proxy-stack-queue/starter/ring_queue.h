#ifndef SDP_W07_RING_QUEUE_H_
#define SDP_W07_RING_QUEUE_H_

#include <cstddef>
#include <stdexcept>
#include <utility>

// Task 2: a FIFO queue in a circular buffer (ring buffer).
//
//   capacity 8, head_ = 6, size_ = 4:
//
//   index:   0   1   2   3   4   5   6   7
//          [ c | d |   |   |   |   | a | b ]
//                  ^ next push      ^ head_ (front)
//
// The element at logical position i (0 = front) lives at physical index
// (head_ + i) % capacity_. Both push and pop are Theta(1): nothing is
// ever shifted. When full, grow to twice the capacity and "unwrap": copy
// the elements to the new buffer in logical order, starting at index 0.
//
// Invariant: 0 <= size_ <= capacity_, head_ < capacity_ (or both 0),
// data_ == nullptr iff capacity_ == 0.
template <typename T>
class RingQueue {
public:
    RingQueue() noexcept = default;
    ~RingQueue() { delete[] data_; }

    RingQueue(const RingQueue&) = delete;             // not needed this week
    RingQueue& operator=(const RingQueue&) = delete;

    bool empty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return capacity_; }

    // Append at the back. Grows (1, then x2) when full.
    void push(const T& value) {
        // TODO
        (void)value;
    }

    // The oldest element. std::out_of_range if empty.
    T& front() {
        // TODO
        throw std::logic_error("TODO: RingQueue::front");
    }

    // The newest element. std::out_of_range if empty.
    T& back() {
        // TODO
        throw std::logic_error("TODO: RingQueue::back");
    }

    // Remove the oldest element. std::out_of_range if empty.
    void pop() {
        // TODO
    }

    // Element at logical position i (0 = front). For the tests.
    const T& at(std::size_t i) const {
        if (i >= size_) {
            throw std::out_of_range("RingQueue::at");
        }
        return data_[(head_ + i) % capacity_];
    }

private:
    void grow() {
        // TODO: new buffer of max(1, 2 * capacity_); copy in logical order;
        // head_ becomes 0
    }

    T* data_ = nullptr;
    std::size_t head_ = 0;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

#endif  // SDP_W07_RING_QUEUE_H_
