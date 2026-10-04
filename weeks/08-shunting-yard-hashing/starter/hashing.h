#ifndef SDP_W08_HASHING_H_
#define SDP_W08_HASHING_H_

#include <cstdint>
#include <string>

// Task 4: hash functions for strings.

// Done - a BAD hash on purpose: the sum of the characters. Every anagram
// ("listen", "silent", "enlist") gets the same value, and all short
// strings land in a tiny range. See the benchmark.
std::uint64_t sumHash(const std::string& s);

// ★ Polynomial hash: s[0]*base^(n-1) + s[1]*base^(n-2) + ... + s[n-1], mod m.
// Compute it with Horner's rule - one multiplication per character:
//   h = (h * base + c) % mod
// Characters are taken as unsigned char. Precondition: 1 <= mod < 2^32.
std::uint64_t polyHash(const std::string& s, std::uint64_t base = 31, std::uint64_t mod = 1'000'000'007);

// ★ FNV-1a, 64 bit (Fowler-Noll-Vo):
//   h = 14695981039346656037  (offset basis)
//   for each byte c:  h ^= c;  h *= 1099511628211  (the FNV prime), mod 2^64
std::uint64_t fnv1a(const std::string& s);

#endif  // SDP_W08_HASHING_H_
