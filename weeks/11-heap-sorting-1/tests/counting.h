#ifndef SDP_W11_TESTS_COUNTING_H_
#define SDP_W11_TESTS_COUNTING_H_

#include <cstddef>

// A "less" that counts how many times it is called. Copies share the
// counter, so it still counts when an algorithm copies the comparator.
struct CountingLess {
    std::size_t* count;
    template <typename T>
    bool operator()(const T& a, const T& b) const {
        ++*count;
        return a < b;
    }
};

// An int that counts assignments and copy/move constructions - "writes".
struct Tracked {
    int v = 0;
    static inline std::size_t writes = 0;

    Tracked() = default;
    Tracked(int x) : v(x) {}  // NOLINT: implicit on purpose, for {3, 1, 2}
    Tracked(const Tracked& o) : v(o.v) { ++writes; }
    Tracked(Tracked&& o) noexcept : v(o.v) { ++writes; }
    Tracked& operator=(const Tracked& o) {
        v = o.v;
        ++writes;
        return *this;
    }
    Tracked& operator=(Tracked&& o) noexcept {
        v = o.v;
        ++writes;
        return *this;
    }
    friend bool operator<(const Tracked& a, const Tracked& b) { return a.v < b.v; }
};

#endif  // SDP_W11_TESTS_COUNTING_H_
