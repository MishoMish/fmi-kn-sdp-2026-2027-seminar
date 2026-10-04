#include <catch_amalgamated.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <random>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include "bst.h"

namespace {

using V = std::vector<int>;

// Exactly these keys, checked with Task 4 only (size + contains), so the
// erase tests do not depend on Tasks 6-7.
bool hasExactly(const BST<int>& t, const V& keys) {
    if (t.size() != keys.size()) return false;
    for (int k : keys)
        if (!t.contains(k)) return false;
    return true;
}

//            50
//         /      \        (the tree used below)
//       30        70
//      /  \      /  \     (the tree used below)
//    20    40  60    80
//         /  \         \  (the tree used below)
//       35    45        90
BST<int> sample() { return BST<int>{50, 30, 70, 20, 40, 60, 80, 35, 45, 90}; }

}  // namespace

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: insert, contains, size", "[task4]") {
    BST<int> t;
    REQUIRE(t.empty());
    REQUIRE(t.insert(5));
    REQUIRE(t.insert(3));
    REQUIRE(t.insert(8));
    REQUIRE_FALSE(t.insert(5));  // already there
    REQUIRE(t.size() == 3);
    REQUIRE(t.checkInvariants());
    REQUIRE(t.contains(3));
    REQUIRE(t.contains(8));
    REQUIRE_FALSE(t.contains(4));
    REQUIRE_FALSE(t.contains(100));
}

TEST_CASE("Task 4: the shape depends on the insertion order", "[task4]") {
    BST<int> t = sample();
    REQUIRE(t.size() == 10);
    REQUIRE(t.height() == 3);
    REQUIRE(t.checkInvariants());

    BST<int> chain;
    for (int i = 0; i < 2000; ++i) chain.insert(i);  // sorted input
    REQUIRE(chain.size() == 2000);
    REQUIRE(chain.height() == 1999);  // a linked list in disguise
    REQUIRE(chain.checkInvariants());
    REQUIRE(chain.contains(1999));
}

TEST_CASE("Task 4: min and max", "[task4]") {
    BST<int> t = sample();
    REQUIRE(t.min() == 20);
    REQUIRE(t.max() == 90);

    BST<int> empty;
    REQUIRE_THROWS_AS(empty.min(), std::out_of_range);
    REQUIRE_THROWS_AS(empty.max(), std::out_of_range);
}

TEST_CASE("Task 4: Compare decides the order", "[task4]") {
    BST<int, std::greater<int>> t;
    for (int x : {5, 1, 9, 3}) t.insert(x);
    REQUIRE(t.min() == 9);  // "smallest" by std::greater
    REQUIRE(t.max() == 1);
    REQUIRE(t.checkInvariants());

    BST<std::string> words;
    for (const char* w : {"tree", "heap", "graph", "array", "list", "heap"}) words.insert(w);
    REQUIRE(words.size() == 5);
    REQUIRE(words.min() == "array");
    REQUIRE(words.max() == "tree");
    REQUIRE(words.contains("graph"));
    REQUIRE_FALSE(words.contains("hash"));
}

TEST_CASE("Task 4: copies are independent", "[task4]") {
    BST<int> a = sample();
    BST<int> b = a;
    b.insert(1000);
    REQUIRE_FALSE(a.contains(1000));
    REQUIRE(b.contains(1000));
    REQUIRE(a.checkInvariants());
    REQUIRE(b.checkInvariants());
}

// ---------------------------------------------------------------- Task 5

TEST_CASE("Task 5: erase a leaf", "[task5]") {
    BST<int> t = sample();
    REQUIRE(t.erase(35));
    REQUIRE_FALSE(t.contains(35));
    REQUIRE(t.size() == 9);
    REQUIRE(t.checkInvariants());
    REQUIRE(hasExactly(t, V{20, 30, 40, 45, 50, 60, 70, 80, 90}));
}

TEST_CASE("Task 5: erase a node with one child", "[task5]") {
    BST<int> t = sample();
    REQUIRE(t.erase(80));  // only a right child (90)
    REQUIRE(t.checkInvariants());
    REQUIRE(t.erase(45));  // now 40 has only a left child (35)
    REQUIRE(t.erase(40));
    REQUIRE(t.checkInvariants());
    REQUIRE(hasExactly(t, V{20, 30, 35, 50, 60, 70, 90}));
}

TEST_CASE("Task 5: erase a node with two children", "[task5]") {
    BST<int> t = sample();
    REQUIRE(t.erase(30));  // successor 35 is deep in the right subtree
    REQUIRE(t.checkInvariants());
    REQUIRE(t.erase(70));  // successor 80 is the right child itself
    REQUIRE(t.checkInvariants());
    REQUIRE(hasExactly(t, V{20, 35, 40, 45, 50, 60, 80, 90}));
}

TEST_CASE("Task 5: erase the root, again and again", "[task5]") {
    // Each key below is the root at the moment it is erased (successor
    // rule): two children, then "right child is the successor", then only
    // a left child (90, 45), and finally a lone root (20).
    BST<int> t = sample();
    V left{20, 30, 35, 40, 45, 50, 60, 70, 80, 90};
    for (int root : {50, 60, 70, 80, 90, 30, 35, 40, 45, 20}) {
        INFO("erasing " << root);
        REQUIRE(t.erase(root));
        left.erase(std::find(left.begin(), left.end(), root));
        REQUIRE(t.checkInvariants());
        REQUIRE(hasExactly(t, left));
    }
    REQUIRE(t.empty());
    REQUIRE(t.height() == -1);
    REQUIRE(t.insert(7));  // still usable
    REQUIRE(t.checkInvariants());
}

TEST_CASE("Task 5: erase a missing key", "[task5]") {
    BST<int> t = sample();
    REQUIRE_FALSE(t.erase(55));
    REQUIRE(t.size() == 10);
    BST<int> empty;
    REQUIRE_FALSE(empty.erase(1));
}

TEST_CASE("Task 5: the successor node moves, its value is not copied", "[task5]") {
    //     30
    //    /  \       (tree)
    //  20    40
    //       /       (tree)
    //     35
    BST<int> t{30, 20, 40, 35};
    t.erase(20);
    t.erase(40);           // now: 30 with right child 35
    // 35 is the max: &max() is the address of the node holding 35
    const int* before = &t.max();
    REQUIRE(*before == 35);
    t.insert(20);          // 30 has two children again: 20 and 35
    REQUIRE(t.erase(30));  // successor of 30 is 35
    REQUIRE(t.checkInvariants());
    REQUIRE(&t.max() == before);  // the same node, relinked
    REQUIRE(t.max() == 35);
}

TEST_CASE("Task 5: random inserts and erases against std::set", "[task5]") {
    std::mt19937 rng(2026);
    BST<int> t;
    std::set<int> ref;
    for (int step = 0; step < 20'000; ++step) {
        const int x = static_cast<int>(rng() % 500);
        if (rng() % 2) {
            REQUIRE(t.insert(x) == ref.insert(x).second);
        } else {
            REQUIRE(t.erase(x) == (ref.erase(x) == 1));
        }
        REQUIRE(t.size() == ref.size());
        if (step % 500 == 0) {
            REQUIRE(t.checkInvariants());
            REQUIRE(hasExactly(t, V(ref.begin(), ref.end())));
        }
    }
    REQUIRE(t.checkInvariants());
}

// ---------------------------------------------------------------- Task 6

TEST_CASE("Task 6: floor and ceiling", "[task6]") {
    BST<int> t = sample();  // 20 30 35 40 45 50 60 70 80 90
    REQUIRE(t.floor(42) == 40);
    REQUIRE(t.ceiling(42) == 45);
    REQUIRE(t.floor(40) == 40);
    REQUIRE(t.ceiling(40) == 40);
    REQUIRE(t.floor(55) == 50);
    REQUIRE(t.ceiling(55) == 60);
    REQUIRE(t.floor(19) == std::nullopt);
    REQUIRE(t.ceiling(19) == 20);
    REQUIRE(t.floor(1000) == 90);
    REQUIRE(t.ceiling(91) == std::nullopt);

    BST<int> empty;
    REQUIRE(empty.floor(1) == std::nullopt);
    REQUIRE(empty.ceiling(1) == std::nullopt);
}

TEST_CASE("Task 6: floor and ceiling against std::set", "[task6]") {
    std::mt19937 rng(42);
    BST<int> t;
    std::set<int> ref;
    for (int i = 0; i < 300; ++i) {
        const int x = static_cast<int>(rng() % 3000);
        t.insert(x);
        ref.insert(x);
    }
    for (int q = -10; q < 3010; ++q) {
        auto ge = ref.lower_bound(q);
        std::optional<int> ceil = ge == ref.end() ? std::nullopt : std::optional<int>(*ge);
        auto gt = ref.upper_bound(q);
        std::optional<int> flo = gt == ref.begin() ? std::nullopt : std::optional<int>(*std::prev(gt));
        INFO("q = " << q);
        REQUIRE(t.ceiling(q) == ceil);
        REQUIRE(t.floor(q) == flo);
    }
}

// ---------------------------------------------------------------- Task 7

TEST_CASE("Task 7: iterator traits", "[task7]") {
    using It = BST<int>::iterator;
    STATIC_REQUIRE(std::is_same_v<std::iterator_traits<It>::iterator_category, std::forward_iterator_tag>);
    STATIC_REQUIRE(std::is_same_v<std::iterator_traits<It>::value_type, int>);
    STATIC_REQUIRE(std::is_same_v<decltype(*std::declval<It>()), const int&>);
}

TEST_CASE("Task 7: range-for visits the keys in sorted order", "[task7]") {
    BST<int> t = sample();
    V seen;
    for (int x : t) seen.push_back(x);
    REQUIRE(seen == V{20, 30, 35, 40, 45, 50, 60, 70, 80, 90});

    BST<int> empty;
    REQUIRE(empty.begin() == empty.end());
}

TEST_CASE("Task 7: works with standard algorithms", "[task7]") {
    BST<int> t = sample();
    REQUIRE(V(t.begin(), t.end()) == V{20, 30, 35, 40, 45, 50, 60, 70, 80, 90});
    REQUIRE(std::distance(t.begin(), t.end()) == 10);
    REQUIRE(std::find(t.begin(), t.end(), 45) != t.end());
    REQUIRE(std::find(t.begin(), t.end(), 46) == t.end());
    REQUIRE(std::is_sorted(t.begin(), t.end()));
    REQUIRE(std::count_if(t.begin(), t.end(), [](int x) { return x % 20 == 0; }) == 4);

    auto it = t.begin();
    REQUIRE(*it++ == 20);
    REQUIRE(*it == 30);
}

TEST_CASE("Task 7: iterating follows Compare and handles chains", "[task7]") {
    BST<int, std::greater<int>> desc;
    for (int x : {5, 1, 9, 3, 7}) desc.insert(x);
    REQUIRE(V(desc.begin(), desc.end()) == V{9, 7, 5, 3, 1});

    BST<int> up, down;
    for (int i = 0; i < 2000; ++i) {
        up.insert(i);         // right chain
        down.insert(1999 - i);  // left chain
    }
    V expected(2000);
    for (int i = 0; i < 2000; ++i) expected[i] = i;
    REQUIRE(V(up.begin(), up.end()) == expected);
    REQUIRE(V(down.begin(), down.end()) == expected);
}

TEST_CASE("Task 7: iterating after random erases", "[task7]") {
    std::mt19937 rng(7);
    BST<int> t;
    std::set<int> ref;
    for (int i = 0; i < 2000; ++i) {
        const int x = static_cast<int>(rng() % 1000);
        t.insert(x);
        ref.insert(x);
    }
    for (int i = 0; i < 700; ++i) {
        const int x = static_cast<int>(rng() % 1000);
        t.erase(x);
        ref.erase(x);
    }
    REQUIRE(V(t.begin(), t.end()) == V(ref.begin(), ref.end()));
}

TEST_CASE("Task 7: erase does not move the other elements", "[task7]") {
    BST<int> t = sample();
    const int* addr35 = &*std::find(t.begin(), t.end(), 35);
    const int* addr80 = &*std::find(t.begin(), t.end(), 80);
    REQUIRE(t.erase(30));  // successor 35: deep, the left child of 40
    REQUIRE(t.erase(70));  // successor 80: the right child itself
    REQUIRE(t.checkInvariants());
    REQUIRE(&*std::find(t.begin(), t.end(), 35) == addr35);
    REQUIRE(&*std::find(t.begin(), t.end(), 80) == addr80);
}
