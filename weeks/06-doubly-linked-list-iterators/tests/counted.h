#ifndef SDP_W06_COUNTED_H_
#define SDP_W06_COUNTED_H_

#include <stdexcept>

// Memory-leak detection by instrumentation: every Counted object that is
// created must be destroyed. If a test ends with Counted::alive != 0, some
// node was never deleted (or deleted twice, if it goes negative).
struct Counted {
    static inline long alive = 0;
    // When > 0, the copy constructor throws on that copy (1 = the next one).
    static inline int throwOnCopy = 0;
    int value = 0;

    Counted(int v = 0) : value(v) { ++alive; }  // NOLINT: implicit on purpose
    Counted(const Counted& o) : value(o.value) {
        if (throwOnCopy > 0 && --throwOnCopy == 0) {
            throw std::runtime_error("Counted: copy failed (on purpose)");
        }
        ++alive;
    }
    Counted& operator=(const Counted& o) {
        value = o.value;
        return *this;
    }
    ~Counted() { --alive; }
    bool operator==(const Counted& o) const { return value == o.value; }
};

#endif  // SDP_W06_COUNTED_H_
