#include <catch_amalgamated.hpp>

#include <algorithm>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "binary_tree.h"

namespace {

using std::nullopt;
using V = std::vector<int>;

// Owns a raw tree for the duration of a test, so a failing REQUIRE does
// not leak (the tests also run under ASan).
struct Tree {
    Node<int>* root;
    explicit Tree(Node<int>* r) : root(r) {}
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;
    ~Tree() { destroy(root); }
};

//          4
//        /   \      (sample)
//       2     6
//      / \     \    (sample)
//     1   3     7
Node<int>* sample() { return fromLevelOrder<int>({4, 2, 6, 1, 3, nullopt, 7}); }

// The expression tree of 3 + 4 * (2 - 1) from week 08, with operators
// stored as their character codes.
Node<int>* expressionTree() {
    auto* minus = new Node<int>('-', new Node<int>(2), new Node<int>(1));
    auto* times = new Node<int>('*', new Node<int>(4), minus);
    return new Node<int>('+', new Node<int>(3), times);
}

std::string asExpression(const V& tokens) {
    std::string s;
    for (int t : tokens) s += (t >= 0 && t <= 9) ? static_cast<char>('0' + t) : static_cast<char>(t);
    return s;
}

// A chain of n nodes, each the RIGHT child of the previous: 0, 1, ..., n-1.
Node<int>* rightChain(int n) {
    Node<int>* root = nullptr;
    for (int i = n - 1; i >= 0; --i) root = new Node<int>(i, nullptr, root);
    return root;
}

// A chain of n nodes, each the LEFT child of the previous: n-1, ..., 1, 0.
Node<int>* leftChain(int n) {
    Node<int>* root = nullptr;
    for (int i = 0; i < n; ++i) root = new Node<int>(i, root, nullptr);
    return root;
}

}  // namespace

TEST_CASE("Given: fromLevelOrder and toString", "[given]") {
    Tree t(sample());
    REQUIRE(toString(t.root) == "4(2(1,3),6(,7))");
    REQUIRE(fromLevelOrder<int>({}) == nullptr);
}

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: size, height, countLeaves", "[task1]") {
    Tree empty(nullptr);
    REQUIRE(size(empty.root) == 0);
    REQUIRE(height(empty.root) == -1);
    REQUIRE(countLeaves(empty.root) == 0);

    Tree one(new Node<int>(42));
    REQUIRE(size(one.root) == 1);
    REQUIRE(height(one.root) == 0);
    REQUIRE(countLeaves(one.root) == 1);

    Tree t(sample());
    REQUIRE(size(t.root) == 6);
    REQUIRE(height(t.root) == 2);
    REQUIRE(countLeaves(t.root) == 3);

    Tree chain(rightChain(100));
    REQUIRE(size(chain.root) == 100);
    REQUIRE(height(chain.root) == 99);
    REQUIRE(countLeaves(chain.root) == 1);
}

TEST_CASE("Task 1: the three depth-first orders", "[task1]") {
    Tree t(sample());
    REQUIRE(preorder(t.root) == V{4, 2, 1, 3, 6, 7});
    REQUIRE(inorder(t.root) == V{1, 2, 3, 4, 6, 7});
    REQUIRE(postorder(t.root) == V{1, 3, 2, 7, 6, 4});

    Tree empty(nullptr);
    REQUIRE(preorder(empty.root).empty());
    REQUIRE(inorder(empty.root).empty());
    REQUIRE(postorder(empty.root).empty());
}

TEST_CASE("Task 1: the expression tree - in-order is infix, post-order is RPN", "[task1]") {
    Tree e(expressionTree());
    REQUIRE(asExpression(inorder(e.root)) == "3+4*2-1");  // brackets are lost!
    REQUIRE(asExpression(postorder(e.root)) == "3421-*+");
    REQUIRE(asExpression(preorder(e.root)) == "+3*4-21");
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: levelOrder", "[task2]") {
    Tree t(sample());
    REQUIRE(levelOrder(t.root) == V{4, 2, 6, 1, 3, 7});
    Tree empty(nullptr);
    REQUIRE(levelOrder(empty.root).empty());

    // fromLevelOrder and levelOrder are inverses on a complete tree
    V values(31);
    std::iota(values.begin(), values.end(), 1);
    std::vector<std::optional<int>> opt(values.begin(), values.end());
    Tree full(fromLevelOrder(opt));
    REQUIRE(levelOrder(full.root) == values);
}

TEST_CASE("Task 2: inorderIterative matches inorder", "[task2]") {
    Tree t(sample());
    REQUIRE(inorderIterative(t.root) == V{1, 2, 3, 4, 6, 7});
    Tree empty(nullptr);
    REQUIRE(inorderIterative(empty.root).empty());

    std::mt19937 rng(9);
    for (int trial = 0; trial < 50; ++trial) {
        // a random shape: random level order with random gaps
        std::vector<std::optional<int>> opt;
        const int n = 1 + static_cast<int>(rng() % 40);
        for (int i = 0; i < n; ++i) {
            if (i > 0 && rng() % 4 == 0) opt.push_back(nullopt);
            else opt.push_back(static_cast<int>(rng() % 1000));
        }
        Tree r(fromLevelOrder(opt));
        INFO(toString(r.root));
        REQUIRE(inorderIterative(r.root) == inorder(r.root));
    }
}

TEST_CASE("Task 2: inorderIterative survives a 1 000 000-level tree", "[task2]") {
    constexpr int n = 1'000'000;
    V expected(n);
    std::iota(expected.begin(), expected.end(), 0);

    Tree right(rightChain(n));
    REQUIRE(inorderIterative(right.root) == expected);

    Tree left(leftChain(n));  // n-1 at the root, 0 deepest: in-order is 0..n-1 too
    REQUIRE(inorderIterative(left.root) == expected);
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: isBST on simple trees", "[task3]") {
    Tree empty(nullptr);
    REQUIRE(isBST(empty.root));
    Tree one(new Node<int>(1));
    REQUIRE(isBST(one.root));
    Tree t(sample());
    REQUIRE(isBST(t.root));

    Tree swapped(fromLevelOrder<int>({4, 6, 2}));
    REQUIRE_FALSE(isBST(swapped.root));

    Tree duplicate(fromLevelOrder<int>({4, 4, 6}));
    REQUIRE_FALSE(isBST(duplicate.root));  // keys must be strictly increasing
}

TEST_CASE("Task 3: isBST - comparing with the children only is not enough", "[task3]") {
    //        5
    //      /   \     (tree)
    //     3     8
    //    / \         (tree)
    //   1   6        <- 6 > 3 (fine for its parent), but 6 > 5 in 5's LEFT subtree
    Tree t(fromLevelOrder<int>({5, 3, 8, 1, 6}));
    REQUIRE_FALSE(isBST(t.root));
    t.root->left->right->value = 4;  // now it is fine
    REQUIRE(isBST(t.root));

    //        5
    //      /   \     (tree)
    //     3     8
    //          / \   (tree)
    //         4   9  <- 4 < 5 in 5's RIGHT subtree
    Tree u(fromLevelOrder<int>({5, 3, 8, nullopt, nullopt, 4, 9}));
    REQUIRE_FALSE(isBST(u.root));
}

TEST_CASE("Task 3: isBST works with strings", "[task3]") {
    Node<std::string>* t = fromLevelOrder<std::string>({"m", "f", "t", "a", "h"});
    REQUIRE(isBST(t));
    t->left->right->value = "z";  // "z" in the left subtree of "m"
    REQUIRE_FALSE(isBST(t));
    destroy(t);
}

TEST_CASE("Task 3: buildBalanced has minimum height", "[task3]") {
    Tree empty(buildBalanced(V{}));
    REQUIRE(empty.root == nullptr);

    for (int n = 1; n <= 300; ++n) {
        V sorted(n);
        std::iota(sorted.begin(), sorted.end(), 10);
        Tree t(buildBalanced(sorted));
        INFO("n = " << n);
        REQUIRE(isBST(t.root));
        REQUIRE(inorder(t.root) == sorted);
        int floorLog2 = 0;
        while ((2 << floorLog2) <= n) ++floorLog2;
        REQUIRE(height(t.root) == floorLog2);
    }
}

TEST_CASE("Task 3: buildBalanced - the middle element is the root", "[task3]") {
    Tree t(buildBalanced(V{1, 2, 3, 4, 5, 6, 7}));
    REQUIRE(levelOrder(t.root) == V{4, 2, 6, 1, 3, 5, 7});
}
