#ifndef SDP_W04_TRACKED_H_
#define SDP_W04_TRACKED_H_

// Element types that count how often they are copied and moved.
// Used by the tests to check WHAT your DynamicArray does, not just the result.

struct Counters {
    long copies = 0;
    long moves = 0;
    void reset() { copies = moves = 0; }
};

// Move operations are noexcept - std::move_if_noexcept will move it.
struct Tracked {
    static Counters counters;
    int value = 0;

    Tracked() = default;
    Tracked(int v) : value(v) {}  // NOLINT: implicit on purpose, for {1, 2, 3}
    Tracked(const Tracked& o) : value(o.value) { ++counters.copies; }
    Tracked(Tracked&& o) noexcept : value(o.value) { o.value = -1; ++counters.moves; }
    Tracked& operator=(const Tracked& o) {
        value = o.value;
        ++counters.copies;
        return *this;
    }
    Tracked& operator=(Tracked&& o) noexcept {
        value = o.value;
        o.value = -1;
        ++counters.moves;
        return *this;
    }
    bool operator==(const Tracked& o) const { return value == o.value; }
};
inline Counters Tracked::counters;

// Same, but the move operations may throw - std::move_if_noexcept copies it.
struct ThrowingMove {
    static Counters counters;
    int value = 0;

    ThrowingMove() = default;
    ThrowingMove(int v) : value(v) {}  // NOLINT
    ThrowingMove(const ThrowingMove& o) : value(o.value) { ++counters.copies; }
    ThrowingMove(ThrowingMove&& o) noexcept(false) : value(o.value) { ++counters.moves; }
    ThrowingMove& operator=(const ThrowingMove& o) {
        value = o.value;
        ++counters.copies;
        return *this;
    }
    ThrowingMove& operator=(ThrowingMove&& o) noexcept(false) {
        value = o.value;
        ++counters.moves;
        return *this;
    }
    bool operator==(const ThrowingMove& o) const { return value == o.value; }
};
inline Counters ThrowingMove::counters;

#endif  // SDP_W04_TRACKED_H_
