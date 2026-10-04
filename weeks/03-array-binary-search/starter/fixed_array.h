#ifndef SDP_W03_FIXED_ARRAY_H_
#define SDP_W03_FIXED_ARRAY_H_

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

// An array with a capacity chosen at construction and fixed afterwards.
// It owns a heap buffer of `capacity` elements; the first `size` of them
// are in use:
//
//   data_ -> [ e0 | e1 | ... | e(size-1) | unused ... ]
//              <---------- size ------->
//              <------------------ capacity ------------>
//
// Invariant: 0 <= size_ <= capacity_, and data_ points to capacity_ elements.
//
// Already implemented: construction, destruction, size queries, operator[],
// iterators, comparison, swap. Your tasks are marked TODO.
template <typename T>
class FixedArray {
public:
    using value_type = T;
    using size_type = std::size_t;
    using iterator = T*;               // a pointer is a perfectly good iterator
    using const_iterator = const T*;

    explicit FixedArray(std::size_t capacity)
        : data_(new T[capacity]{}), size_(0), capacity_(capacity) {}

    FixedArray(std::size_t capacity, std::initializer_list<T> values) : FixedArray(capacity) {
        if (values.size() > capacity) {
            throw std::length_error("FixedArray: too many initial values");
        }
        std::copy(values.begin(), values.end(), data_);
        size_ = values.size();
    }

    ~FixedArray() { delete[] data_; }

    // Task 3: deep copy. The result has the same capacity and its OWN buffer.
    FixedArray(const FixedArray& other)
        : data_(new T[other.capacity_]{}), size_(0), capacity_(other.capacity_) {
        // TODO: copy the elements and the size
    }

    // Task 3: deep copy assignment. Must work for a = a, and when the two
    // arrays have different capacities (the left one takes the right one's).
    FixedArray& operator=(const FixedArray& other) {
        // TODO (hint: copy-and-swap)
        (void)other;
        return *this;
    }

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }
    bool full() const noexcept { return size_ == capacity_; }

    // No bounds check - like std::vector::operator[].
    T& operator[](std::size_t index) { return data_[index]; }
    const T& operator[](std::size_t index) const { return data_[index]; }

    // Task 1: bounds-checked access. Throw std::out_of_range if index >= size().
    T& at(std::size_t index) {
        // TODO
        return data_[index];
    }
    const T& at(std::size_t index) const {
        // TODO
        return data_[index];
    }

    T& front() { return data_[0]; }
    T& back() { return data_[size_ - 1]; }

    // Task 1: append at the end. Throw std::length_error if the array is full.
    void pushBack(const T& value) {
        // TODO
        (void)value;
    }

    // Task 1: remove the last element. Throw std::out_of_range if empty.
    void popBack() {
        // TODO
    }

    // Task 2: insert `value` so that it ends up at position `index`;
    // elements from index on move one place right. index == size() appends.
    // Throw std::out_of_range if index > size(), std::length_error if full.
    void insertAt(std::size_t index, const T& value) {
        // TODO
        (void)index;
        (void)value;
    }

    // Task 2: remove the element at `index`; later elements move one place left.
    // Throw std::out_of_range if index >= size().
    void removeAt(std::size_t index) {
        // TODO
        (void)index;
    }

    iterator begin() noexcept { return data_; }
    iterator end() noexcept { return data_ + size_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator end() const noexcept { return data_ + size_; }

    // Equal if the elements in use are equal; capacity does not matter.
    bool operator==(const FixedArray& other) const {
        return size_ == other.size_ && std::equal(begin(), end(), other.begin());
    }
    bool operator!=(const FixedArray& other) const { return !(*this == other); }

    void swap(FixedArray& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;
};

#endif  // SDP_W03_FIXED_ARRAY_H_
