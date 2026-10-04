#ifndef SDP_W08_PROBING_HASH_SET_H_
#define SDP_W08_PROBING_HASH_SET_H_

#include <cstddef>
#include <functional>
#include <vector>

// Task 6 ★★★: a hash set with OPEN ADDRESSING and LINEAR PROBING.
//
// All keys live directly in one array of slots. A key goes to slot
// hash(key) % capacity; if that slot is taken, try the next one, and the
// next (wrapping around), until a free slot is found.
//
// Each slot is in one of three states:
//   Empty    - never used: a search can stop here
//   Full     - holds a key
//   Deleted  - a "tombstone": the key was erased. A search must NOT stop
//              here (the key it looks for may lie further on), but an
//              insert may reuse the slot.
//
// Keep (full + deleted) / capacity <= 0.5 by rehashing to twice the size
// (tombstones are dropped during the rehash).
template <typename K, typename Hash = std::hash<K>>
class ProbingHashSet {
public:
    explicit ProbingHashSet(std::size_t capacity = 8, Hash hash = Hash())
        : slots_(capacity < 2 ? 2 : capacity), hash_(hash) {}

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return slots_.size(); }
    std::size_t tombstones() const noexcept { return deleted_; }

    // Add key; return true if it was not there yet.
    bool insert(const K& key) {
        // TODO
        (void)key;
        return false;
    }

    bool contains(const K& key) const {
        // TODO: probe from the home slot; stop at Empty; skip Deleted
        (void)key;
        return false;
    }

    // Remove key (leave a tombstone); return true if it was there.
    bool erase(const K& key) {
        // TODO
        (void)key;
        return false;
    }

    // Number of slots inspected by the last contains() call (for the tests
    // and the benchmark).
    std::size_t lastProbes() const noexcept { return lastProbes_; }

private:
    enum class State { Empty, Full, Deleted };
    struct Slot {
        K key{};
        State state = State::Empty;
    };

    std::size_t home(const K& key) const { return hash_(key) % slots_.size(); }

    void rehash(std::size_t newCapacity) {
        // TODO: re-insert every Full slot into a fresh array; tombstones vanish
        (void)newCapacity;
    }

    std::vector<Slot> slots_;
    std::size_t size_ = 0;
    std::size_t deleted_ = 0;
    Hash hash_;
    mutable std::size_t lastProbes_ = 0;
};

#endif  // SDP_W08_PROBING_HASH_SET_H_
