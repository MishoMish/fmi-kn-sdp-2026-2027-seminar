#include <catch_amalgamated.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "avl_tree.h"

namespace {

using V = std::vector<int>;
using N = AvlNode<int>;

// Hand-built trees for Tasks 1-2. The fields are filled in here, NOT with
// your update(), so these tests check update() instead of relying on it.
N* fix(N* n) {
    if (!n) return n;
    fix(n->left);
    fix(n->right);
    const int hl = n->left ? n->left->height : -1;
    const int hr = n->right ? n->right->height : -1;
    n->height = 1 + std::max(hl, hr);
    n->size = 1 + (n->left ? n->left->size : 0) + (n->right ? n->right->size : 0);
    return n;
}

N* mk(int v, N* l = nullptr, N* r = nullptr) {
    N* n = new N(v);
    n->left = l;
    n->right = r;
    return n;
}

void freeAll(N* n) {
    if (!n) return;
    freeAll(n->left);
    freeAll(n->right);
    delete n;
}

V levelOrder(const N* root) {
    V out;
    std::queue<const N*> q;
    if (root) q.push(root);
    while (!q.empty()) {
        const N* n = q.front();
        q.pop();
        out.push_back(n->value);
        if (n->left) q.push(n->left);
        if (n->right) q.push(n->right);
    }
    return out;
}

V inorder(const N* n) {
    if (!n) return {};
    V out = inorder(n->left);
    out.push_back(n->value);
    V right = inorder(n->right);
    out.insert(out.end(), right.begin(), right.end());
    return out;
}

// The AVL height bound (Knuth): h < 1.4405 log2(n + 2) - 0.3277.
bool withinAvlBound(int h, std::size_t n) {
    return h < 1.4405 * std::log2(static_cast<double>(n) + 2.0) - 0.3277;
}

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: update recomputes height and size from the children", "[task1]") {
    //      5
    //     / \       (tree)
    //    3   8
    //         \     (tree)
    //          9
    N* t = mk(5, fix(mk(3)), fix(mk(8, nullptr, mk(9))));
    update(t);
    REQUIRE(t->height == 2);
    REQUIRE(t->size == 4);

    N* leaf = mk(1);
    leaf->height = 7;  // stale values
    leaf->size = 42;
    update(leaf);
    REQUIRE(leaf->height == 0);
    REQUIRE(leaf->size == 1);
    freeAll(t);
    freeAll(leaf);
}

TEST_CASE("Task 1: rotateRight", "[task1]") {
    //        4              2
    //       / \            / \      (before / after)
    //      2   5   ==>    1   4
    //     / \                / \    (before / after)
    //    1   3              3   5
    N* y = fix(mk(4, mk(2, mk(1), mk(3)), mk(5)));
    N* x = y->left;
    N* b = x->right;
    N* r = rotateRight(y);
    REQUIRE(r == x);  // the same node, not a copy
    REQUIRE(r->right == y);
    REQUIRE(y->left == b);
    REQUIRE(levelOrder(r) == V{2, 1, 4, 3, 5});
    REQUIRE(inorder(r) == V{1, 2, 3, 4, 5});
    REQUIRE(y->height == 1);
    REQUIRE(y->size == 3);
    REQUIRE(r->height == 2);
    REQUIRE(r->size == 5);
    freeAll(r);
}

TEST_CASE("Task 1: rotateLeft", "[task1]") {
    N* x = fix(mk(2, mk(1), mk(4, mk(3), mk(5))));
    N* y = x->right;
    N* r = rotateLeft(x);
    REQUIRE(r == y);
    REQUIRE(r->left == x);
    REQUIRE(levelOrder(r) == V{4, 2, 5, 1, 3});
    REQUIRE(inorder(r) == V{1, 2, 3, 4, 5});
    REQUIRE(x->height == 1);
    REQUIRE(x->size == 3);
    REQUIRE(r->height == 2);
    REQUIRE(r->size == 5);
    freeAll(r);
}

TEST_CASE("Task 1: rotations with empty subtrees", "[task1]") {
    N* r = rotateRight(fix(mk(2, mk(1))));  // 2(1) -> 1(,2)
    REQUIRE(levelOrder(r) == V{1, 2});
    REQUIRE(r->left == nullptr);
    REQUIRE(r->height == 1);
    REQUIRE(r->right->height == 0);
    r = rotateLeft(r);  // and back
    REQUIRE(levelOrder(r) == V{2, 1});
    REQUIRE(r->size == 2);
    freeAll(r);
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: the four cases", "[task2]") {
    SECTION("LL: one right rotation") {
        N* r = rebalance(fix(mk(3, mk(2, mk(1)))));
        REQUIRE(levelOrder(r) == V{2, 1, 3});
        REQUIRE(r->height == 1);
        freeAll(r);
    }
    SECTION("RR: one left rotation") {
        N* r = rebalance(fix(mk(1, nullptr, mk(2, nullptr, mk(3)))));
        REQUIRE(levelOrder(r) == V{2, 1, 3});
        freeAll(r);
    }
    SECTION("LR: left at the child, then right") {
        N* r = rebalance(fix(mk(3, mk(1, nullptr, mk(2)))));
        REQUIRE(levelOrder(r) == V{2, 1, 3});
        REQUIRE(r->size == 3);
        freeAll(r);
    }
    SECTION("RL: right at the child, then left") {
        N* r = rebalance(fix(mk(1, nullptr, mk(3, mk(2)))));
        REQUIRE(levelOrder(r) == V{2, 1, 3});
        freeAll(r);
    }
}

TEST_CASE("Task 2: LR with subtrees", "[task2]") {
    //          6                  4
    //         / \                / \        (before / after)
    //        2   7     ==>      2   6
    //       / \                / \ / \      (before / after)
    //      1   4              1  3 5  7
    //         / \                           (before)
    //        3   5
    N* r = rebalance(fix(mk(6, mk(2, mk(1), mk(4, mk(3), mk(5))), mk(7))));
    REQUIRE(levelOrder(r) == V{4, 2, 6, 1, 3, 5, 7});
    REQUIRE(r->height == 2);
    REQUIRE(r->size == 7);
    freeAll(r);
}

TEST_CASE("Task 2: child balanced (only after erase) - a single rotation", "[task2]") {
    //        6                 3
    //       /                 / \      (before / after)
    //      3        ==>      2   6
    //     / \                   /      (before / after)
    //    2   4                 4
    N* r = rebalance(fix(mk(6, mk(3, mk(2), mk(4)))));
    REQUIRE(levelOrder(r) == V{3, 2, 6, 4});
    freeAll(r);
}

TEST_CASE("Task 2: a balanced node is only updated", "[task2]") {
    N* n = fix(mk(2, mk(1), mk(3)));
    n->height = 9;  // stale
    N* r = rebalance(n);
    REQUIRE(r == n);
    REQUIRE(r->height == 1);
    REQUIRE(levelOrder(r) == V{2, 1, 3});
    freeAll(r);
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: insert 1, 2, 3 - the rotation from last week's exit question", "[task3]") {
    AVLTree<int> t;
    REQUIRE(t.insert(1));
    REQUIRE(t.insert(2));
    REQUIRE(t.insert(3));
    REQUIRE_FALSE(t.insert(2));
    REQUIRE(t.size() == 3);
    REQUIRE(t.root()->value == 2);
    REQUIRE(t.height() == 1);
    REQUIRE(t.checkInvariants());
}

TEST_CASE("Task 3: sorted input gives perfect trees", "[task3]") {
    AVLTree<int> t;
    for (int i = 1; i <= 7; ++i) t.insert(i);
    REQUIRE(levelOrder(t.root()) == V{4, 2, 6, 1, 3, 5, 7});

    AVLTree<int> big;
    for (int i = 1; i < (1 << 15); ++i) big.insert(i);  // 2^15 - 1 keys
    REQUIRE(big.checkInvariants());
    REQUIRE(big.height() == 14);
}

TEST_CASE("Task 3: 100 000 sorted and reverse-sorted inserts stay logarithmic", "[task3]") {
    AVLTree<int> up, down;
    for (int i = 0; i < 100'000; ++i) {
        up.insert(i);
        down.insert(-i);
    }
    REQUIRE(up.size() == 100'000);
    REQUIRE(up.checkInvariants());
    REQUIRE(down.checkInvariants());
    REQUIRE(up.height() <= 17);  // last week's BST: 99 999
    REQUIRE(withinAvlBound(down.height(), down.size()));
    REQUIRE(up.contains(99'999));
    REQUIRE_FALSE(up.contains(100'000));
}

TEST_CASE("Task 3: random inserts against std::set", "[task3]") {
    std::mt19937 rng(10);
    AVLTree<int> t;
    std::set<int> ref;
    for (int i = 0; i < 5000; ++i) {
        const int x = static_cast<int>(rng() % 3000);
        REQUIRE(t.insert(x) == ref.insert(x).second);
        if (i % 250 == 0) REQUIRE(t.checkInvariants());
    }
    REQUIRE(t.checkInvariants());
    REQUIRE(t.size() == ref.size());
    REQUIRE(t.toVector() == V(ref.begin(), ref.end()));
    REQUIRE(withinAvlBound(t.height(), t.size()));
}

TEST_CASE("Task 3: Compare and other key types", "[task3]") {
    AVLTree<int, std::greater<int>> desc;
    for (int i = 0; i < 100; ++i) desc.insert(i);
    REQUIRE(desc.checkInvariants());
    REQUIRE(desc.min() == 99);
    REQUIRE(desc.toVector().front() == 99);

    AVLTree<std::string> words{"tree", "heap", "graph", "array", "list", "heap", "stack"};
    REQUIRE(words.size() == 6);
    REQUIRE(words.checkInvariants());
    REQUIRE(words.toVector() == std::vector<std::string>{"array", "graph", "heap", "list", "stack", "tree"});

    AVLTree<std::string> copy = words;
    copy.insert("queue");
    REQUIRE_FALSE(words.contains("queue"));
    REQUIRE(copy.checkInvariants());
}

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: erase without rotations", "[task4]") {
    AVLTree<int> t{4, 2, 6, 1, 3, 5, 7};
    REQUIRE(t.erase(1));  // leaf
    REQUIRE(t.erase(2));  // one child (3)
    REQUIRE(t.checkInvariants());
    REQUIRE(t.erase(6));  // two children: successor 7
    REQUIRE(t.checkInvariants());
    REQUIRE(t.toVector() == V{3, 4, 5, 7});
    REQUIRE_FALSE(t.erase(6));
    REQUIRE(t.size() == 4);
}

TEST_CASE("Task 4: erase that needs a single rotation", "[task4]") {
    AVLTree<int> t{2, 1, 3, 4};  // 2(1, 3(, 4))
    REQUIRE(t.erase(1));          // 2 becomes -2: RR
    REQUIRE(t.checkInvariants());
    REQUIRE(levelOrder(t.root()) == V{3, 2, 4});
}

TEST_CASE("Task 4: erase that needs a double rotation", "[task4]") {
    AVLTree<int> t{2, 1, 4, 3};  // 2(1, 4(3))
    REQUIRE(t.erase(1));          // RL
    REQUIRE(t.checkInvariants());
    REQUIRE(levelOrder(t.root()) == V{3, 2, 4});
}

TEST_CASE("Task 4: the successor node is relinked, not copied", "[task4]") {
    AVLTree<int> t{2, 1, 3};
    const int* three = &t.max();
    REQUIRE(t.erase(2));  // successor 3 takes the root's place
    REQUIRE(t.checkInvariants());
    REQUIRE(t.root()->value == 3);
    REQUIRE(&t.max() == three);
}

TEST_CASE("Task 4: erase everything, in several orders", "[task4]") {
    for (int order = 0; order < 3; ++order) {
        AVLTree<int> t;
        V keys(1000);
        for (int i = 0; i < 1000; ++i) keys[i] = i;
        for (int k : keys) t.insert(k);
        if (order == 1) std::reverse(keys.begin(), keys.end());
        if (order == 2) std::shuffle(keys.begin(), keys.end(), std::mt19937(order));
        for (std::size_t i = 0; i < keys.size(); ++i) {
            REQUIRE(t.erase(keys[i]));
            if (i % 50 == 0) {
                REQUIRE(t.checkInvariants());
                REQUIRE(t.size() == keys.size() - i - 1);
            }
        }
        REQUIRE(t.empty());
        REQUIRE(t.height() == -1);
    }
}

TEST_CASE("Task 4: random inserts and erases against std::set", "[task4]") {
    std::mt19937 rng(2026);
    AVLTree<int> t;
    std::set<int> ref;
    for (int step = 0; step < 20'000; ++step) {
        const int x = static_cast<int>(rng() % 300);
        if (rng() % 2) REQUIRE(t.insert(x) == ref.insert(x).second);
        else REQUIRE(t.erase(x) == (ref.erase(x) == 1));
        REQUIRE(t.checkInvariants());  // after EVERY step
        REQUIRE(t.size() == ref.size());
    }
    REQUIRE(t.toVector() == V(ref.begin(), ref.end()));
}

// ---------------------------------------------------------------- Task 5

TEST_CASE("Task 5: rank, select, countRange", "[task5]") {
    AVLTree<int> t{50, 10, 90, 30, 70, 20, 40, 60, 80, 100};
    REQUIRE(t.rank(10) == 0);
    REQUIRE(t.rank(55) == 5);
    REQUIRE(t.rank(60) == 5);
    REQUIRE(t.rank(5) == 0);
    REQUIRE(t.rank(1000) == 10);

    REQUIRE(t.select(0) == 10);
    REQUIRE(t.select(4) == 50);
    REQUIRE(t.select(9) == 100);
    REQUIRE_THROWS_AS(t.select(10), std::out_of_range);

    REQUIRE(t.countRange(20, 50) == 4);
    REQUIRE(t.countRange(25, 25) == 0);
    REQUIRE(t.countRange(30, 30) == 1);
    REQUIRE(t.countRange(50, 20) == 0);
    REQUIRE(t.countRange(0, 1000) == 10);
}

TEST_CASE("Task 5: sizes stay correct through rotations and erases", "[task5]") {
    std::mt19937 rng(5);
    AVLTree<int> t;
    std::set<int> ref;
    for (int i = 0; i < 3000; ++i) {
        const int x = static_cast<int>(rng() % 5000);
        t.insert(x);
        ref.insert(x);
    }
    for (int i = 0; i < 1000; ++i) {
        const int x = static_cast<int>(rng() % 5000);
        t.erase(x);
        ref.erase(x);
    }
    const V sorted(ref.begin(), ref.end());
    for (std::size_t k = 0; k < sorted.size(); ++k) REQUIRE(t.select(k) == sorted[k]);
    for (int q = -5; q < 5005; q += 7) {
        const auto r = static_cast<std::size_t>(std::lower_bound(sorted.begin(), sorted.end(), q) - sorted.begin());
        REQUIRE(t.rank(q) == r);
    }
    for (int lo = 0; lo < 5000; lo += 311) {
        const int hi = lo + 777;
        const auto expected = static_cast<std::size_t>(std::upper_bound(sorted.begin(), sorted.end(), hi) -
                                                       std::lower_bound(sorted.begin(), sorted.end(), lo));
        REQUIRE(t.countRange(lo, hi) == expected);
    }
}
