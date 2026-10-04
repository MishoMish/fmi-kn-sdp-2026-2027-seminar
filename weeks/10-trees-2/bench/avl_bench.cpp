// Insert n int keys (the even numbers 0, 2, ..., 2n-2), then look up all
// of 0, 1, ..., 2n-1 in random order (half present, half absent).
// Nanoseconds per insert and per lookup, and the AVL height.
// Your AVL tree against last week's plain BST and std::set (red-black).
//
// "random" inserts the keys shuffled; "sorted" in increasing order (the
// plain BST only up to n = 20 000: it is Theta(n^2) in total).
//
//   cmake --preset release
//   cmake --build --preset release --target w10_bench
//   ./build/release/w10_bench            (add --csv for machine-readable output)

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <numeric>
#include <random>
#include <set>
#include <vector>

#include "avl_tree.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

// Last week's BST in a nutshell (insert and contains, iterative, no erase).
class PlainBst {
    struct Node {
        int value;
        Node* left = nullptr;
        Node* right = nullptr;
    };

public:
    ~PlainBst() {
        std::vector<Node*> todo;
        if (root_) todo.push_back(root_);
        while (!todo.empty()) {
            Node* n = todo.back();
            todo.pop_back();
            if (n->left) todo.push_back(n->left);
            if (n->right) todo.push_back(n->right);
            delete n;
        }
    }
    void insert(int x) {
        Node** link = &root_;
        while (*link) {
            if (x < (*link)->value) link = &(*link)->left;
            else if ((*link)->value < x) link = &(*link)->right;
            else return;
        }
        *link = new Node{x};
    }
    std::size_t count(int x) const {  // the same loop as AVLTree::contains
        const Node* n = root_;
        while (n) {
            if (x < n->value) n = n->left;
            else if (n->value < x) n = n->right;
            else return 1;
        }
        return 0;
    }

private:
    Node* root_ = nullptr;
};

struct Avl : AVLTree<int> {
    std::size_t count(int x) const { return contains(x) ? 1 : 0; }
};

struct Result {
    double insertNs;  // per insert
    double findNs;    // per lookup (half present, half absent)
};

template <typename Set>
Result run(Set& s, const std::vector<int>& in, const std::vector<int>& queries) {
    long long found = 0;
    const auto t0 = Clock::now();
    for (int k : in) s.insert(k);
    const auto t1 = Clock::now();
    for (int k : queries) found += static_cast<long long>(s.count(k));
    const auto t2 = Clock::now();
    g_sink = found;
    return {std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(in.size()),
            std::chrono::duration<double, std::nano>(t2 - t1).count() / static_cast<double>(queries.size())};
}

void print(bool csv, int n, const char* name, const char* order, Result r, int height) {
    if (csv) {
        std::printf("%d,%s,%s,%.1f,%.1f,%d\n", n, name, order, r.insertNs, r.findNs, height);
    } else {
        char h[16] = "-";
        if (height >= 0) std::snprintf(h, sizeof h, "%d", height);
        std::printf("%8d | %-9s | %-6s | %9.1f | %9.1f | %6s\n", n, name, order, r.insertNs, r.findNs, h);
    }
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    if (csv) {
        std::printf("n,structure,order,insert_ns,find_ns,height\n");
    } else {
        std::printf("ns per insert / per lookup (half present, half absent), int keys\n\n");
        std::printf("%8s | %-9s | %-6s | %9s | %9s | %6s\n", "n", "structure", "order", "insert", "find", "height");
        std::printf("---------+-----------+--------+-----------+-----------+-------\n");
    }
    std::mt19937 rng(42);
    for (int n : {1000, 10'000, 20'000, 100'000, 1'000'000}) {
        std::vector<int> sorted(n);
        for (int i = 0; i < n; ++i) sorted[i] = 2 * i;
        std::vector<int> shuffled = sorted;
        std::shuffle(shuffled.begin(), shuffled.end(), rng);
        std::vector<int> queries(2 * static_cast<std::size_t>(n));
        std::iota(queries.begin(), queries.end(), 0);
        std::shuffle(queries.begin(), queries.end(), rng);

        for (int k = 0; k < 2; ++k) {
            const std::vector<int>& in = k == 0 ? shuffled : sorted;
            const char* order = k == 0 ? "random" : "sorted";
            {
                Avl t;
                const Result r = run(t, in, queries);
                print(csv, n, "avl", order, r, t.height());
            }
            {
                std::set<int> t;
                const Result r = run(t, in, queries);
                print(csv, n, "std::set", order, r, -1);
            }
            if (k == 0 || n <= 20'000) {
                PlainBst t;
                const Result r = run(t, in, queries);
                print(csv, n, "plain_bst", order, r, -1);
            }
        }
        if (!csv) std::printf("---------+-----------+--------+-----------+-----------+-------\n");
    }
    return 0;
}
