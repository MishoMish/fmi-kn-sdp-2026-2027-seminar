#ifndef SDP_W15_BITS_H_
#define SDP_W15_BITS_H_

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

// Bit tricks on UNSIGNED integers - shifts and & | ^ ~ are well defined
// there; on signed ints some of them are not (or were not before C++20).
// Bit 0 is the least significant (rightmost) bit.
//
//   x & (1u << i)    is bit i set?        x | (1u << i)    set bit i
//   x & ~(1u << i)   clear bit i          x ^ (1u << i)    flip bit i
//   x & (x - 1)      x without its lowest set bit
//   x & -x           only the lowest set bit (in unsigned arithmetic)

using u32 = std::uint32_t;
using u64 = std::uint64_t;

// ---------------------------------------------------------------- Task 1 ★

// "1011" for 11; "0" for 0. No leading zeros.
inline std::string toBinary(u64 x) {
    // TODO: take x % 2 (x & 1), shift right, reverse at the end
    (void)x;
    return "";
}

// The inverse; throws std::invalid_argument on anything but '0'/'1', on an
// empty string, or if the value does not fit into 64 bits.
inline u64 fromBinary(const std::string& s) {
    // TODO: x = (x << 1) | bit
    (void)s;
    return 0;
}

inline bool getBit(u32 x, unsigned i) {
    // TODO
    (void)x;
    (void)i;
    return false;
}
inline u32 setBit(u32 x, unsigned i) {
    // TODO
    (void)i;
    return x;
}
inline u32 clearBit(u32 x, unsigned i) {
    // TODO
    (void)i;
    return x;
}
inline u32 toggleBit(u32 x, unsigned i) {
    // TODO
    (void)i;
    return x;
}

// ---------------------------------------------------------------- Task 2 ★

// Number of set bits. Kernighan: x &= x - 1 removes the lowest set bit -
// as many iterations as there are ones, not 64.
inline unsigned popcount(u64 x) {
    // TODO
    (void)x;
    return 99;
}

// 1, 2, 4, 8, ... - exactly one bit set. (0 is not a power of two.)
inline bool isPowerOfTwo(u64 x) {
    // TODO: one line
    (void)x;
    return false;
}

// Index of the lowest set bit (0 for 1, 3 for 8, 1 for 6); 64 for x == 0.
inline unsigned countTrailingZeros(u64 x) {
    // TODO
    (void)x;
    return 99;
}

// ---------------------------------------------------------------- Task 3 ★★

// The smallest power of two >= x (1 for 0 and 1). "Smear" the highest set
// bit of x - 1 to the right with x |= x >> 1, >> 2, >> 4, ..., then add 1.
// Throws std::overflow_error if the answer does not fit (x > 2^63).
inline u64 nextPowerOfTwo(u64 x) {
    // TODO
    (void)x;
    return 0;
}

// Bits in reverse order: bit 0 <-> bit 31, bit 1 <-> bit 30, ...
inline u32 reverseBits(u32 x) {
    // TODO
    (void)x;
    return 0;
}

// The element that appears exactly once; every other appears exactly twice.
// One pass, O(1) memory: a ^ a == 0 and a ^ 0 == a.
inline int findUnique(const std::vector<int>& v) {
    // TODO
    (void)v;
    return -1;
}

// ---------------------------------------------------------------- Task 4 ★★

// All 2^n subsets of v (n <= 20): the subset number `mask` contains v[i]
// iff bit i of mask is set. Order: mask = 0, 1, 2, ..., 2^n - 1; inside a
// subset, the elements in the order of v.
inline std::vector<std::vector<int>> allSubsets(const std::vector<int>& v) {
    // TODO
    (void)v;
    return {};
}

// The n-bit Gray code: 2^n numbers, every two neighbours (also the last and
// the first) differ in exactly one bit, starting with 0. g(i) = i ^ (i >> 1).
inline std::vector<u32> grayCode(unsigned n) {
    // TODO
    (void)n;
    return {};
}

// ---------------------------------------------------------------- Task 5 ★★★

// Number of ways to place n queens on an n x n board so that none attacks
// another (n <= 16). Row by row, with three bitmasks of attacked COLUMNS,
// "/" diagonals and "\" diagonals; the free columns in the current row are
// ~(cols | d1 | d2) & ((1 << n) - 1); take them one by one with x & -x.
inline u64 nQueens(unsigned n) {
    // TODO
    (void)n;
    return 0;
}

#endif  // SDP_W15_BITS_H_
