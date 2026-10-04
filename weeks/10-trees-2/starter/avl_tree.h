#ifndef SDP_W10_AVL_TREE_H_
#define SDP_W10_AVL_TREE_H_

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>

// An AVL tree: a binary search tree in which, for EVERY node, the heights
// of the two subtrees differ by at most 1. That keeps the height below
// 1.44 log2(n + 2), so every operation is Theta(log n) - whatever the
// order of the input.
//
// Every node stores two extra numbers about its own subtree:
//   height - edges on the longest path down (a leaf has 0, nullptr has -1)
//   size   - number of nodes (for rank/select, Task 5)
// Both are recomputed from the children by update() whenever the shape
// below a node changes.
//
// No parent pointers this week: everything is recursive. With a height of
// at most ~28 for a million keys, recursion is perfectly safe again.

template <typename T>
struct AvlNode {
    T value;
    AvlNode* left = nullptr;
    AvlNode* right = nullptr;
    int height = 0;
    std::size_t size = 1;

    explicit AvlNode(const T& v) : value(v) {}
};

// ---------------------------------------------------------------- given

template <typename T>
int heightOf(const AvlNode<T>* n) {
    return n ? n->height : -1;
}

template <typename T>
std::size_t sizeOf(const AvlNode<T>* n) {
    return n ? n->size : 0;
}

// height(left) - height(right): +2 means "left too tall", -2 "right too tall".
template <typename T>
int balanceFactor(const AvlNode<T>* n) {
    return heightOf(n->left) - heightOf(n->right);
}

// ---------------------------------------------------------------- Task 1 ★

// Recompute n->height and n->size from n's children (whose own fields are
// assumed correct).
template <typename T>
void update(AvlNode<T>* n) {
    // TODO
    (void)n;
}

// Right rotation around y; returns the new root of this subtree.
//
//          y                x         before / after
//         / \              / \        (edges)
//        x   C    ==>     A   y
//       / \                  / \      (edges)
//      A   B                B   C
//
// In-order stays A x B y C. Only B changes parent. Update y FIRST (it is
// now below x), then x.
template <typename T>
AvlNode<T>* rotateRight(AvlNode<T>* y) {
    // TODO
    return y;
}

// The mirror image: x's right child y becomes the root.
template <typename T>
AvlNode<T>* rotateLeft(AvlNode<T>* x) {
    // TODO
    return x;
}

// ---------------------------------------------------------------- Task 2 ★★

// Called on the way back up after an insert or erase below n. The subtrees
// of n are valid AVL trees; n itself may be off by 2. Update n, and if
// |balanceFactor(n)| == 2 restore the balance with one single or one
// double rotation. Returns the (possibly new) root of this subtree.
//
//   bf(n) == +2 and bf(n->left) >= 0  -> LL: rotateRight(n)
//   bf(n) == +2 and bf(n->left) <  0  -> LR: n->left = rotateLeft(n->left), then rotateRight(n)
//   bf(n) == -2 and bf(n->right) <= 0 -> RR: rotateLeft(n)
//   bf(n) == -2 and bf(n->right) >  0 -> RL: mirror of LR
template <typename T>
AvlNode<T>* rebalance(AvlNode<T>* n) {
    // TODO
    return n;
}

// ---------------------------------------------------------------- the tree

template <typename T, typename Compare = std::less<T>>
class AVLTree {
public:
    using Node = AvlNode<T>;

    AVLTree() = default;
    explicit AVLTree(Compare comp) : comp_(std::move(comp)) {}
    AVLTree(std::initializer_list<T> values) {
        for (const T& v : values) insert(v);
    }

    AVLTree(const AVLTree& other) : root_(clone(other.root_)), comp_(other.comp_) {}
    AVLTree(AVLTree&& other) noexcept : root_(other.root_), comp_(std::move(other.comp_)) { other.root_ = nullptr; }
    AVLTree& operator=(AVLTree other) noexcept {
        std::swap(root_, other.root_);
        std::swap(comp_, other.comp_);
        return *this;
    }
    ~AVLTree() { destroy(root_); }

    std::size_t size() const noexcept { return sizeOf(root_); }
    bool empty() const noexcept { return root_ == nullptr; }
    int height() const noexcept { return heightOf(root_); }

    bool contains(const T& x) const {
        const Node* cur = root_;
        while (cur) {
            if (comp_(x, cur->value)) cur = cur->left;
            else if (comp_(cur->value, x)) cur = cur->right;
            else return true;
        }
        return false;
    }

    const T& min() const {
        if (!root_) throw std::out_of_range("AVLTree::min: empty tree");
        const Node* n = root_;
        while (n->left) n = n->left;
        return n->value;
    }
    const T& max() const {
        if (!root_) throw std::out_of_range("AVLTree::max: empty tree");
        const Node* n = root_;
        while (n->right) n = n->right;
        return n->value;
    }

    // The keys in sorted order.
    std::vector<T> toVector() const {
        std::vector<T> out;
        out.reserve(size());
        collect(root_, out);
        return out;
    }

    // Used by the tests after every operation: BST order, |balance| <= 1
    // everywhere, and every stored height and size equal to the real one.
    bool checkInvariants() const {
        int h = 0;
        std::size_t s = 0;
        return check(root_, nullptr, nullptr, h, s);
    }

    // For the tests and the figures: the root (read only).
    const Node* root() const noexcept { return root_; }

    // ------------------------------------------------------------ Task 3 ★

    // Insert x if absent; return true if inserted.
    bool insert(const T& x) {
        bool inserted = false;
        root_ = insertAt(root_, x, inserted);
        return inserted;
    }

    // ------------------------------------------------------------ Task 4 ★★★

    // Remove x if present; return true if removed.
    bool erase(const T& x) {
        bool erased = false;
        root_ = eraseAt(root_, x, erased);
        return erased;
    }

    // ------------------------------------------------------------ Task 5 ★★

    // Number of keys < x (x need not be in the tree).
    std::size_t rank(const T& x) const {
        // TODO: walk down; going right skips the left subtree AND the node
        (void)x;
        return 0;
    }

    // The k-th smallest key, 0-based: select(0) == min().
    // Throws std::out_of_range if k >= size().
    const T& select(std::size_t k) const {
        // TODO: compare k with sizeOf(left)
        (void)k;
        throw std::logic_error("TODO: AVLTree::select");
    }

    // Number of keys in [lo, hi] (0 if hi < lo). Use rank and contains.
    std::size_t countRange(const T& lo, const T& hi) const {
        // TODO
        (void)lo;
        (void)hi;
        return 0;
    }

private:
    // Task 3: insert x into the subtree n; return the subtree's new root.
    // Same as last week's recursive insert - plus "return rebalance(n);"
    // on the way back up.
    Node* insertAt(Node* n, const T& x, bool& inserted) {
        // TODO
        (void)x;
        (void)inserted;
        return n;
    }

    // Task 4: unlink the smallest node of the subtree n (rebalancing on the
    // way up) and store it in minNode; return the subtree's new root.
    Node* extractMin(Node* n, Node*& minNode) {
        // TODO
        minNode = nullptr;
        return n;
    }

    // Task 4: erase x from the subtree n; return the subtree's new root.
    // Last week's three cases, with the successor RELINKED (extractMin on
    // the right subtree), then rebalance every node on the way up.
    Node* eraseAt(Node* n, const T& x, bool& erased) {
        // TODO
        (void)x;
        (void)erased;
        return n;
    }

    static void collect(const Node* n, std::vector<T>& out) {
        if (!n) return;
        collect(n->left, out);
        out.push_back(n->value);
        collect(n->right, out);
    }

    bool check(const Node* n, const T* lo, const T* hi, int& h, std::size_t& s) const {
        if (!n) {
            h = -1;
            s = 0;
            return true;
        }
        if (lo && !comp_(*lo, n->value)) return false;
        if (hi && !comp_(n->value, *hi)) return false;
        int hl = 0, hr = 0;
        std::size_t sl = 0, sr = 0;
        if (!check(n->left, lo, &n->value, hl, sl) || !check(n->right, &n->value, hi, hr, sr)) return false;
        h = 1 + std::max(hl, hr);
        s = 1 + sl + sr;
        return hl - hr >= -1 && hl - hr <= 1 && n->height == h && n->size == s;
    }

    static Node* clone(const Node* n) {
        if (!n) return nullptr;
        Node* c = new Node(n->value);
        c->height = n->height;
        c->size = n->size;
        try {
            c->left = clone(n->left);
            c->right = clone(n->right);
        } catch (...) {
            destroy(c);
            throw;
        }
        return c;
    }

    static void destroy(Node* n) {  // recursion depth = height <= ~1.44 log2 n
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    Node* root_ = nullptr;
    Compare comp_;
};

#endif  // SDP_W10_AVL_TREE_H_
