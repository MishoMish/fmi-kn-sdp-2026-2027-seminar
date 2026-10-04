#ifndef SDP_W09_BST_H_
#define SDP_W09_BST_H_

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

// A binary search tree with set semantics, like std::set: unique keys,
// ordered by Compare (std::less<T> by default), iterated in sorted order.
//
// Every node also knows its PARENT. That costs one pointer per node and a
// little care in insert/erase, and it buys two things std::set needs:
//   - an iterator that is just a node pointer (++ walks up via parent);
//   - erase that relinks nodes, so pointers/iterators to OTHER elements
//     stay valid.
//
// "a < b" below means comp_(a, b). Two keys are equal when neither is less.
//
// NOT balanced (that is next week): every operation is Theta(height), and
// the height is anywhere between floor(log2 n) and n - 1.
template <typename T, typename Compare = std::less<T>>
class BST {
    struct Node {
        T value;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;

        Node(const T& v, Node* p) : value(v), parent(p) {}
    };

public:
    // ------------------------------------------------------------ iterator

    // Task 7 ★★★: in-order iteration. The iterator is a node pointer;
    // end() is nullptr. Elements are const: changing a key in place would
    // break the ordering.
    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator() = default;

        reference operator*() const { return node_->value; }
        pointer operator->() const { return &node_->value; }

        // Move to the in-order SUCCESSOR:
        //   - if there is a right subtree: its leftmost node;
        //   - otherwise: climb while we are a RIGHT child; the parent we
        //     reach from a LEFT child is next (nullptr if none: end()).
        const_iterator& operator++() {
            // TODO
            throw std::logic_error("TODO: BST::const_iterator::operator++");
        }

        const_iterator operator++(int) {
            const_iterator old = *this;
            ++*this;
            return old;
        }

        friend bool operator==(const const_iterator& a, const const_iterator& b) { return a.node_ == b.node_; }
        friend bool operator!=(const const_iterator& a, const const_iterator& b) { return a.node_ != b.node_; }

    private:
        friend class BST;
        explicit const_iterator(const Node* n) : node_(n) {}

        const Node* node_ = nullptr;
    };
    using iterator = const_iterator;

    // Task 7: the leftmost node (the smallest key), or end() if empty.
    const_iterator begin() const {
        // TODO
        return end();
    }
    const_iterator end() const { return const_iterator(nullptr); }

    // ------------------------------------------------------------ given

    BST() = default;
    explicit BST(Compare comp) : comp_(std::move(comp)) {}
    BST(std::initializer_list<T> values) {
        for (const T& v : values) insert(v);
    }

    BST(const BST& other) : root_(cloneTree(other.root_)), size_(other.size_), comp_(other.comp_) {}
    BST(BST&& other) noexcept : root_(other.root_), size_(other.size_), comp_(std::move(other.comp_)) {
        other.root_ = nullptr;
        other.size_ = 0;
    }
    BST& operator=(BST other) noexcept {  // copy-and-swap (week 04)
        swap(other);
        return *this;
    }
    ~BST() { clear(); }

    void swap(BST& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
        std::swap(comp_, other.comp_);
    }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    // Iterative: no recursion, so a degenerate tree cannot overflow the stack.
    void clear() noexcept {
        std::vector<Node*> todo;
        if (root_) todo.push_back(root_);
        while (!todo.empty()) {
            Node* n = todo.back();
            todo.pop_back();
            if (n->left) todo.push_back(n->left);
            if (n->right) todo.push_back(n->right);
            delete n;
        }
        root_ = nullptr;
        size_ = 0;
    }

    // Edges on the longest root-to-leaf path; -1 if empty. Level by level
    // with a queue (Task 2's idea), so it also works on a 1 000 000-node chain.
    int height() const {
        if (!root_) return -1;
        int levels = 0;
        std::queue<const Node*> level;
        level.push(root_);
        while (!level.empty()) {
            ++levels;
            for (std::size_t k = level.size(); k > 0; --k) {
                const Node* n = level.front();
                level.pop();
                if (n->left) level.push(n->left);
                if (n->right) level.push(n->right);
            }
        }
        return levels - 1;
    }

    // Used by the tests after every operation: keys strictly increasing
    // in-order, every child's parent pointer points back, root's parent is
    // nullptr, and size() matches the node count.
    bool checkInvariants() const {
        if (root_ && root_->parent) return false;
        std::vector<const Node*> stack;
        const Node* cur = root_;
        const Node* prev = nullptr;
        std::size_t count = 0;
        while (cur || !stack.empty()) {
            while (cur) {
                if (cur->left && cur->left->parent != cur) return false;
                if (cur->right && cur->right->parent != cur) return false;
                stack.push_back(cur);
                cur = cur->left;
            }
            cur = stack.back();
            stack.pop_back();
            if (prev && !comp_(prev->value, cur->value)) return false;
            prev = cur;
            ++count;
            cur = cur->right;
        }
        return count == size_;
    }

    // ------------------------------------------------------------ Task 4 ★

    // Insert value if it is not there yet; return true if inserted.
    // Walk down from the root as in a search, remembering the parent; the
    // new node becomes the parent's left or right child. Iterative, please:
    // the tests insert 2 000 sorted keys.
    bool insert(const T& value) {
        // TODO
        (void)value;
        return false;
    }

    bool contains(const T& value) const {
        // TODO: walk down; left if value < node, right if node < value
        (void)value;
        return false;
    }

    // The smallest / largest key in Compare's order.
    // Throws std::out_of_range if the tree is empty.
    const T& min() const {
        // TODO: leftmost node
        throw std::logic_error("TODO: BST::min");
    }
    const T& max() const {
        // TODO: rightmost node
        throw std::logic_error("TODO: BST::max");
    }

    // ------------------------------------------------------------ Task 5 ★★

    // Remove value; return true if it was there. Three cases:
    //   - no children: unlink it from its parent;
    //   - one child: the child takes its place;
    //   - two children: its in-order successor (leftmost of the right
    //     subtree) takes its place.
    // RELINK nodes, do not copy values: the test "erase keeps other
    // elements in place" checks that the successor's address is unchanged,
    // as std::set guarantees. transplant() is the building block.
    bool erase(const T& value) {
        // TODO
        (void)value;
        return false;
    }

    // ------------------------------------------------------------ Task 6 ★★

    // The largest key <= value / the smallest key >= value, if any.
    // One walk down from the root: remember the best candidate so far.
    std::optional<T> floor(const T& value) const {
        // TODO
        (void)value;
        return std::nullopt;
    }
    std::optional<T> ceiling(const T& value) const {
        // TODO
        (void)value;
        return std::nullopt;
    }

private:
    // Put subtree v where subtree u is: u's parent (or root_) now points to
    // v, and v's parent is u's parent. u's own pointers are NOT changed.
    // v may be nullptr.
    void transplant(Node* u, Node* v) {
        // TODO (Task 5)
        (void)u;
        (void)v;
    }

    // Copies the shape exactly, parent pointers included. Iterative.
    static Node* cloneTree(const Node* src) {
        if (!src) return nullptr;
        Node* root = new Node(src->value, nullptr);
        std::vector<std::pair<const Node*, Node*>> todo{{src, root}};
        try {
            while (!todo.empty()) {
                auto [s, d] = todo.back();
                todo.pop_back();
                if (s->left) {
                    d->left = new Node(s->left->value, d);
                    todo.push_back({s->left, d->left});
                }
                if (s->right) {
                    d->right = new Node(s->right->value, d);
                    todo.push_back({s->right, d->right});
                }
            }
        } catch (...) {
            BST tmp;  // frees what was built so far
            tmp.root_ = root;
            throw;
        }
        return root;
    }

    Node* root_ = nullptr;
    std::size_t size_ = 0;
    Compare comp_;
};

#endif  // SDP_W09_BST_H_
