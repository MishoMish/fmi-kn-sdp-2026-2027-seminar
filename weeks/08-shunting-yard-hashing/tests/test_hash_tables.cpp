#include <catch_amalgamated.hpp>

#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "chained_hash_map.h"
#include "probing_hash_set.h"

namespace {

// Every key in the same bucket: the worst possible hash. A correct table
// still works - just slowly.
struct ConstantHash {
    std::size_t operator()(int) const { return 42; }
};

// Keys k and k + 8 always collide in an 8-slot table.
struct IdentityHash {
    std::size_t operator()(int k) const { return static_cast<std::size_t>(k); }
};

}  // namespace

// ---------------------------------------------------------------- Task 5

TEST_CASE("Task 5: insert, find, contains", "[task5]") {
    ChainedHashMap<std::string, int> m;
    REQUIRE(m.insert("tree", 1));
    REQUIRE(m.insert("heap", 2));
    REQUIRE_FALSE(m.insert("tree", 99));  // already there: value unchanged
    REQUIRE(m.size() == 2);
    REQUIRE(*m.find("tree") == 1);
    REQUIRE(m.find("graph") == nullptr);
    REQUIRE(m.contains("heap"));
    *m.find("heap") = 20;
    REQUIRE(*m.find("heap") == 20);
}

TEST_CASE("Task 5: operator[] inserts a default value", "[task5]") {
    ChainedHashMap<std::string, int> counts;
    for (const char* w : {"a", "b", "a", "c", "a", "b"}) {
        ++counts[w];
    }
    REQUIRE(counts.size() == 3);
    REQUIRE(counts["a"] == 3);
    REQUIRE(counts["b"] == 2);
    REQUIRE(counts["c"] == 1);
}

TEST_CASE("Task 5: erase", "[task5]") {
    ChainedHashMap<int, int, IdentityHash> m(8);
    for (int k : {1, 9, 17, 2}) m.insert(k, k * 10);  // 1, 9, 17 share a bucket
    REQUIRE(m.erase(9));
    REQUIRE_FALSE(m.erase(9));
    REQUIRE_FALSE(m.contains(9));
    REQUIRE(*m.find(1) == 10);
    REQUIRE(*m.find(17) == 170);
    REQUIRE(m.size() == 3);
}

TEST_CASE("Task 5: rehash keeps the load factor <= 1 and keeps every key", "[task5]") {
    ChainedHashMap<int, int> m(4);
    for (int i = 0; i < 10'000; ++i) {
        m.insert(i, i * i);
        REQUIRE(m.loadFactor() <= ChainedHashMap<int, int>::maxLoadFactor());
    }
    REQUIRE(m.bucketCount() >= 10'000);
    for (int i = 0; i < 10'000; ++i) {
        REQUIRE(m.find(i) != nullptr);
        REQUIRE(*m.find(i) == i * i);
    }
}

TEST_CASE("Task 5: correct even with a terrible hash", "[task5]") {
    ChainedHashMap<int, int, ConstantHash> m;
    for (int i = 0; i < 300; ++i) m.insert(i, -i);
    REQUIRE(m.longestChain() == 300);  // one chain: Theta(n) per lookup
    for (int i = 0; i < 300; ++i) REQUIRE(*m.find(i) == -i);
    REQUIRE(m.erase(150));
    REQUIRE_FALSE(m.contains(150));
}

TEST_CASE("Task 5: random operations behave like std::unordered_map", "[task5]") {
    std::mt19937 rng(2026);
    ChainedHashMap<int, int> m;
    std::unordered_map<int, int> model;
    for (int step = 0; step < 20'000; ++step) {
        const int k = static_cast<int>(rng() % 500);
        const int op = static_cast<int>(rng() % 3);
        if (op == 0) {
            REQUIRE(m.insert(k, step) == model.emplace(k, step).second);
        } else if (op == 1) {
            REQUIRE(m.erase(k) == (model.erase(k) == 1));
        } else {
            const int* got = m.find(k);
            auto it = model.find(k);
            REQUIRE((got != nullptr) == (it != model.end()));
            if (got) REQUIRE(*got == it->second);
        }
        REQUIRE(m.size() == model.size());
    }
}

// ---------------------------------------------------------------- Task 6

TEST_CASE("Task 6: probing set basics", "[task6]") {
    ProbingHashSet<std::string> s;
    REQUIRE(s.insert("tree"));
    REQUIRE(s.insert("heap"));
    REQUIRE_FALSE(s.insert("tree"));
    REQUIRE(s.contains("tree"));
    REQUIRE_FALSE(s.contains("graph"));
    REQUIRE(s.size() == 2);
}

TEST_CASE("Task 6: erase leaves a tombstone that searches skip", "[task6]") {
    ProbingHashSet<int, IdentityHash> s(16);
    s.insert(1);   // slot 1
    s.insert(17);  // collides with 1 -> slot 2
    s.insert(33);  // -> slot 3
    REQUIRE(s.erase(17));
    REQUIRE(s.tombstones() == 1);
    // If erase had made slot 2 Empty, the search for 33 would stop there.
    REQUIRE(s.contains(33));
    REQUIRE_FALSE(s.contains(17));
    REQUIRE(s.insert(17));  // may reuse the tombstone
    REQUIRE(s.contains(17));
    REQUIRE(s.size() == 3);
}

TEST_CASE("Task 6: load stays <= 0.5 and rehash drops tombstones", "[task6]") {
    ProbingHashSet<int> s(4);
    for (int i = 0; i < 5000; ++i) {
        s.insert(i);
        REQUIRE(2 * (s.size() + s.tombstones()) <= s.capacity());
    }
    for (int i = 0; i < 5000; i += 2) s.erase(i);
    for (int i = 5000; i < 20'000; ++i) s.insert(i);  // forces rehashes
    for (int i = 0; i < 5000; ++i) REQUIRE(s.contains(i) == (i % 2 == 1));
    for (int i = 5000; i < 20'000; ++i) REQUIRE(s.contains(i));
}

TEST_CASE("Task 6: random operations behave like std::unordered_set", "[task6]") {
    std::mt19937 rng(7);
    ProbingHashSet<int> s;
    std::unordered_set<int> model;
    for (int step = 0; step < 30'000; ++step) {
        const int k = static_cast<int>(rng() % 400);
        switch (rng() % 3) {
            case 0: REQUIRE(s.insert(k) == model.insert(k).second); break;
            case 1: REQUIRE(s.erase(k) == (model.erase(k) == 1)); break;
            default: REQUIRE(s.contains(k) == (model.count(k) == 1));
        }
        REQUIRE(s.size() == model.size());
    }
}
