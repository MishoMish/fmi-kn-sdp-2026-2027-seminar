#ifndef SDP_W04_DYNAMIC_ARRAY_H_
#define SDP_W04_DYNAMIC_ARRAY_H_

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

// A resizable array - a simplified std::vector.
//
//   data_ -> [ e0 | e1 | ... | e(size-1) | spare ... ]
//              <---------- size ------->
//              <------------------ capacity --------->
//
// When pushBack finds no spare slot, the array REALLOCATES: a new buffer
// of twice the capacity, elements moved over, old buffer freed. That makes
// pushBack amortized Theta(1).
//
// Invariant: 0 <= size_ <= capacity_; data_ is nullptr iff capacity_ == 0,
// otherwise it points to capacity_ elements owned by this object.
//
// Simplification compared to std::vector: the buffer is new T[capacity],
// so the spare slots hold default-constructed T objects (see notes, §5.4).
template <typename T>
class DynamicArray {
public:
    using value_type = T;
    using size_type = std::size_t;
    using iterator = T*;
    using const_iterator = const T*;

    DynamicArray() noexcept = default;

    DynamicArray(std::initializer_list<T> values) {
        reserve(values.size());
        for (const T& v : values) {
            pushBack(v);
        }
    }

    ~DynamicArray() { delete[] data_; }

    // ---- Task 2: the rule of five ---------------------------------------

    // Deep copy: same size, own buffer (its capacity may be just size()).
    DynamicArray(const DynamicArray& other) {
        // TODO
        (void)other;
    }

    // Steal other's buffer; leave other empty (size 0, capacity 0).
    // No allocation, no element is copied or moved.
    DynamicArray(DynamicArray&& other) noexcept {
        // TODO
        (void)other;
    }

    DynamicArray& operator=(const DynamicArray& other) {
        // TODO (copy-and-swap, as in week 03)
        (void)other;
        return *this;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        // TODO: release what we own, take other's buffer, leave other empty
        (void)other;
        return *this;
    }

    // ---- queries (done) -------------------------------------------------

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }
    T* data() noexcept { return data_; }
    const T* data() const noexcept { return data_; }

    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }

    T& at(std::size_t i) {
        if (i >= size_) throw std::out_of_range("DynamicArray::at");
        return data_[i];
    }
    const T& at(std::size_t i) const {
        if (i >= size_) throw std::out_of_range("DynamicArray::at");
        return data_[i];
    }

    T& front() { return data_[0]; }
    T& back() { return data_[size_ - 1]; }

    iterator begin() noexcept { return data_; }
    iterator end() noexcept { return data_ + size_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator end() const noexcept { return data_ + size_; }

    // ---- Task 1: growth --------------------------------------------------

    // Make room for at least `newCapacity` elements. Never shrinks.
    // Afterwards capacity() == max(old capacity, newCapacity).
    void reserve(std::size_t newCapacity) {
        // TODO: if it is bigger than the current capacity, reallocate(newCapacity)
        (void)newCapacity;
    }

    // Append a copy of `value`. If there is no spare slot, the new capacity
    // is 1 for an empty array and 2 * capacity() otherwise.
    // Careful: `value` may refer to an element of THIS array (a.pushBack(a[0]))
    // - do not read it after its buffer has been freed.
    void pushBack(const T& value) {
        // TODO
        (void)value;
    }

    // Task 3: the same, but move `value` in instead of copying it.
    // Until then it falls back to copying, so Task 1 works on its own.
    void pushBack(T&& value) {
        // TODO Task 3: move instead of copy
        pushBack(static_cast<const T&>(value));
    }

    // Remove the last element. Throw std::out_of_range if empty.
    // Reset the freed slot to T{} so it releases what it owns (e.g. a string).
    void popBack() {
        // TODO
    }

    // ---- Task 3: size control ------------------------------------------

    // New size n: extra elements are T{}; elements beyond n are reset to T{}.
    // Grows the capacity only when n > capacity() (to max(n, 2 * capacity())).
    void resize(std::size_t n) {
        // TODO
        (void)n;
    }

    // Capacity becomes exactly size() (and data() becomes nullptr if empty).
    void shrinkToFit() {
        // TODO
    }

    void swap(DynamicArray& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    bool operator==(const DynamicArray& other) const {
        return size_ == other.size_ && std::equal(begin(), end(), other.begin());
    }
    bool operator!=(const DynamicArray& other) const { return !(*this == other); }

private:
    // Task 1 (Task 4 for the move_if_noexcept part): allocate a buffer of
    // exactly newCapacity elements (newCapacity >= size_), transfer the
    // size_ elements into it, free the old buffer. newCapacity == 0 means
    // "no buffer" (data_ = nullptr).
    //
    // Task 4: transfer with std::move_if_noexcept - elements are MOVED if
    // T's move constructor is noexcept, COPIED otherwise.
    void reallocate(std::size_t newCapacity) {
        // TODO
        (void)newCapacity;
    }

    T* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

#endif  // SDP_W04_DYNAMIC_ARRAY_H_
