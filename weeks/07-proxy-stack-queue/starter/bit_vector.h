#ifndef SDP_W07_BIT_VECTOR_H_
#define SDP_W07_BIT_VECTOR_H_

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

// Task 4: the Proxy pattern in its most common C++ form.
//
// BitVector stores n booleans in n bits - 64 per std::uint64_t word, so
// 8x less memory than bool[] (and what std::vector<bool> does).
//
// The catch: a single bit has no address, so operator[] cannot return
// bool&. Instead it returns a small PROXY object, BitRef, that remembers
// WHICH word and WHICH bit, and behaves like a reference:
//
//   bool b = v[3];      // BitRef -> bool        (operator bool)
//   v[3] = true;        // BitRef = bool         (operator=(bool))
//   v[3] = v[5];        // BitRef = BitRef       (copies the BIT, not the proxy)
//   v[3].flip();
class BitVector {
public:
    class BitRef {
    public:
        // Read the bit.
        operator bool() const {  // NOLINT: implicit on purpose - it is the point of a proxy
            // TODO
            return false;
        }

        // Write the bit.
        BitRef& operator=(bool value) {
            // TODO
            (void)value;
            return *this;
        }

        // v[i] = v[j] must copy the VALUE of bit j into bit i. The
        // compiler-generated version would instead copy the proxy's members
        // (making this proxy point at bit j) and leave bit i unchanged.
        BitRef& operator=(const BitRef& other) {
            // TODO: one line, using the two functions above
            (void)other;
            return *this;
        }

        void flip() {
            // TODO
        }

    private:
        friend class BitVector;
        BitRef(std::uint64_t* word, std::uint64_t mask) : word_(word), mask_(mask) {}

        std::uint64_t* word_;
        std::uint64_t mask_;  // exactly one bit set: the bit this proxy stands for
    };

    explicit BitVector(std::size_t n, bool value = false)
        : words_((n + 63) / 64, value ? ~std::uint64_t{0} : 0), n_(n) {}

    std::size_t size() const noexcept { return n_; }
    std::size_t bytes() const noexcept { return words_.size() * sizeof(std::uint64_t); }

    // Bit i lives in word i / 64, at position i % 64.
    // std::out_of_range if i >= size().
    BitRef operator[](std::size_t i) {
        // TODO
        (void)i;
        throw std::logic_error("TODO: BitVector::operator[]");
    }

    bool operator[](std::size_t i) const {
        // TODO
        (void)i;
        throw std::logic_error("TODO: BitVector::operator[] const");
    }

    // How many bits are 1. Only bits < size() count, even if the
    // constructor set the unused high bits of the last word.
    std::size_t count() const {
        // TODO
        return 0;
    }

private:
    std::vector<std::uint64_t> words_;
    std::size_t n_;
};

#endif  // SDP_W07_BIT_VECTOR_H_
