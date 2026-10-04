#include "hashing.h"

std::uint64_t sumHash(const std::string& s) {
    std::uint64_t h = 0;
    for (unsigned char c : s) {
        h += c;
    }
    return h;
}

std::uint64_t polyHash(const std::string& s, std::uint64_t base, std::uint64_t mod) {
    // TODO
    (void)s;
    (void)base;
    (void)mod;
    return 0;
}

std::uint64_t fnv1a(const std::string& s) {
    // TODO
    (void)s;
    return 0;
}
