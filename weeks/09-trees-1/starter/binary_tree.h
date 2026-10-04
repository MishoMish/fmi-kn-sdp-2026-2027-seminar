#ifndef SDP_W09_BINARY_TREE_H_
#define SDP_W09_BINARY_TREE_H_

#include <cstddef>
#include <optional>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

// A plain binary tree: a node with two pointers, and free functions that
// take the root. No class around it, so every function works on any shape
// (and on any subtree: a child pointer is the root of a smaller tree).
//
//   fromLevelOrder<int>({4, 2, 6, 1, 3, std::nullopt, 7}) builds
//
//          4          <- root, depth 0
//        /   \        (edges)
//       2     6       <- depth 1
//      / \     \      (edges)
//     1   3     7     <- depth 2; leaves 1, 3, 7
//
// Conventions used by the tests:
//   - the empty tree is nullptr;
//   - height(empty) == -1, height(single node) == 0 (edges, not nodes);
//   - a BST has STRICTLY increasing keys in-order (no duplicates).
template <typename T>
struct Node {
    T value;
    Node* left = nullptr;
    Node* right = nullptr;

    explicit Node(const T& v, Node* l = nullptr, Node* r = nullptr) : value(v), left(l), right(r) {}
};

// ---------------------------------------------------------------- given

// Builds a tree from its level order, nullopt marking a missing child
// (the format LeetCode uses). Children of a missing node are not listed.
template <typename T>
Node<T>* fromLevelOrder(const std::vector<std::optional<T>>& values) {
    if (values.empty() || !values[0]) return nullptr;
    Node<T>* root = new Node<T>(*values[0]);
    std::queue<Node<T>*> parents;
    parents.push(root);
    std::size_t i = 1;
    while (i < values.size() && !parents.empty()) {  // extra values are ignored
        Node<T>* p = parents.front();
        parents.pop();
        if (values[i]) parents.push(p->left = new Node<T>(*values[i]));
        ++i;
        if (i < values.size() && values[i]) parents.push(p->right = new Node<T>(*values[i]));
        ++i;
    }
    return root;
}

// Frees every node. Iterative on purpose: a recursive version would need
// one stack frame per level, and a degenerate tree of 1 000 000 nodes has
// 1 000 000 levels.
template <typename T>
void destroy(Node<T>* root) {
    std::vector<Node<T>*> todo;
    if (root) todo.push_back(root);
    while (!todo.empty()) {
        Node<T>* n = todo.back();
        todo.pop_back();
        if (n->left) todo.push_back(n->left);
        if (n->right) todo.push_back(n->right);
        delete n;
    }
}

// "4(2(1,3),6(,7))": value, then (left,right) if there are children.
// Handy in test failure messages.
inline std::string toString(const Node<int>* root) {
    if (!root) return "";
    std::string s = std::to_string(root->value);
    if (root->left || root->right) s += "(" + toString(root->left) + "," + toString(root->right) + ")";
    return s;
}

// ---------------------------------------------------------------- Task 1 ★

// Number of nodes.
template <typename T>
std::size_t size(const Node<T>* root) {
    // TODO: empty tree -> 0; otherwise 1 + size(left) + size(right)
    (void)root;
    return 0;
}

// Edges on the longest root-to-leaf path; -1 for the empty tree.
template <typename T>
int height(const Node<T>* root) {
    // TODO
    (void)root;
    return -2;
}

// Nodes with no children.
template <typename T>
std::size_t countLeaves(const Node<T>* root) {
    // TODO
    (void)root;
    return 0;
}

// The three depth-first orders. Each appends to out; the one-argument
// versions below are ready and just call them.
template <typename T>
void preorder(const Node<T>* root, std::vector<T>& out) {
    // TODO: node, left, right
    (void)root;
    (void)out;
}

template <typename T>
void inorder(const Node<T>* root, std::vector<T>& out) {
    // TODO: left, node, right
    (void)root;
    (void)out;
}

template <typename T>
void postorder(const Node<T>* root, std::vector<T>& out) {
    // TODO: left, right, node
    (void)root;
    (void)out;
}

template <typename T>
std::vector<T> preorder(const Node<T>* root) {
    std::vector<T> out;
    preorder(root, out);
    return out;
}

template <typename T>
std::vector<T> inorder(const Node<T>* root) {
    std::vector<T> out;
    inorder(root, out);
    return out;
}

template <typename T>
std::vector<T> postorder(const Node<T>* root) {
    std::vector<T> out;
    postorder(root, out);
    return out;
}

// ---------------------------------------------------------------- Task 2 ★★

// Breadth-first: level 0, then level 1 left to right, ... Use a queue.
template <typename T>
std::vector<T> levelOrder(const Node<T>* root) {
    // TODO: std::queue<const Node<T>*>
    (void)root;
    return {};
}

// In-order WITHOUT recursion: an explicit stack of the nodes whose left
// subtree we are still inside. The tests run it on a 1 000 000-level chain,
// where the recursive version may run out of call stack.
template <typename T>
std::vector<T> inorderIterative(const Node<T>* root) {
    // TODO: std::stack<const Node<T>*>
    //   go left as far as possible, pushing every node;
    //   pop one, output it, then continue from its right child
    (void)root;
    return {};
}

// ---------------------------------------------------------------- Task 3 ★★

// Is the tree a binary SEARCH tree: every key in the left subtree of a node
// is < its key, every key in the right subtree is > its key?
// Comparing a node only with its two children is NOT enough - see the
// "grandchild" test. Carry the allowed interval (lo, hi) down instead;
// nullptr means "no bound on that side".
template <typename T>
bool isBSTWithin(const Node<T>* root, const T* lo, const T* hi) {
    // TODO
    (void)root;
    (void)lo;
    (void)hi;
    return false;
}

template <typename T>
bool isBST(const Node<T>* root) {
    return isBSTWithin<T>(root, nullptr, nullptr);
}

// A BST of minimum height from a SORTED vector: the middle element becomes
// the root, the two halves become the subtrees. Range is [lo, hi).
template <typename T>
Node<T>* buildBalanced(const std::vector<T>& sorted, std::size_t lo, std::size_t hi) {
    // TODO
    (void)sorted;
    (void)lo;
    (void)hi;
    return nullptr;
}

template <typename T>
Node<T>* buildBalanced(const std::vector<T>& sorted) {
    return buildBalanced(sorted, 0, sorted.size());
}

#endif  // SDP_W09_BINARY_TREE_H_
