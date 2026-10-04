#ifndef SDP_W14_GRAPHS2_H_
#define SDP_W14_GRAPHS2_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

// Weighted graphs: every edge has a non-negative integer weight (a length,
// a cost, a time). Vertices 0 .. n-1, adjacency lists of (to, weight).

struct Edge {
    int to;
    long long w;
};

class WeightedGraph {
public:
    explicit WeightedGraph(std::size_t n, bool directed = false) : adj_(n), directed_(directed) {}

    std::size_t vertexCount() const noexcept { return adj_.size(); }
    bool directed() const noexcept { return directed_; }

    void addEdge(int u, int v, long long w) {
        check(u);
        check(v);
        if (w < 0) throw std::invalid_argument("WeightedGraph: negative weight");
        adj_[u].push_back({v, w});
        if (!directed_ && u != v) adj_[v].push_back({u, w});
    }

    const std::vector<Edge>& neighbors(int u) const {
        check(u);
        return adj_[u];
    }

private:
    void check(int u) const {
        if (u < 0 || static_cast<std::size_t>(u) >= adj_.size()) throw std::out_of_range("WeightedGraph: no such vertex");
    }

    std::vector<std::vector<Edge>> adj_;
    bool directed_;
};

// "Unreachable" distance.
constexpr long long kInf = std::numeric_limits<long long>::max();

// ---------------------------------------------------------------- Task 1 ★★

// Dijkstra from s: dist[v] = length of a shortest path s -> v (kInf if
// unreachable). A min-heap of (distance, vertex) - std::priority_queue with
// std::greater. "Lazy deletion": when a shorter path to v is found, push the
// new pair and leave the old one in the heap; when you pop a pair whose
// distance is larger than dist[v], it is stale - skip it.
inline std::vector<long long> dijkstra(const WeightedGraph& g, int s) {
    // TODO
    (void)g;
    (void)s;
    return {};
}

// A shortest path s -> t (s first, t last), or empty if unreachable.
// Dijkstra that remembers parent[v].
inline std::vector<int> dijkstraPath(const WeightedGraph& g, int s, int t) {
    // TODO
    (void)g;
    (void)s;
    (void)t;
    return {};
}

// ---------------------------------------------------------------- Task 2 ★

// Union-find (disjoint set union) over elements 0 .. n-1. Every set is a
// tree; the root is the set's representative.
//   find:  follow parent to the root, then make every node on the way point
//          straight to the root (PATH COMPRESSION);
//   unite: attach the root of the SMALLER tree under the root of the larger
//          (UNION BY SIZE). Return false if they were already together.
// With both, any sequence of operations is almost O(1) each.
class UnionFind {
public:
    explicit UnionFind(std::size_t n) : parent_(n), size_(n, 1), sets_(n) {
        std::iota(parent_.begin(), parent_.end(), 0);
    }

    int find(int x) {
        // TODO
        return x;
    }

    bool unite(int a, int b) {
        // TODO
        (void)a;
        (void)b;
        return false;
    }

    bool connected(int a, int b) { return find(a) == find(b); }
    std::size_t setCount() const noexcept { return sets_; }
    std::size_t setSize(int x) { return size_[static_cast<std::size_t>(find(x))]; }

    // Given, for the tests: the largest number of parent steps from any
    // element to its root, WITHOUT compressing anything. Union by size alone
    // keeps it <= log2 n.
    std::size_t height() const {
        std::size_t best = 0;
        std::vector<std::size_t> depth(parent_.size(), static_cast<std::size_t>(-1));
        for (std::size_t x = 0; x < parent_.size(); ++x) {
            std::vector<std::size_t> path;
            std::size_t y = x;
            while (depth[y] == static_cast<std::size_t>(-1) && static_cast<std::size_t>(parent_[y]) != y) {
                path.push_back(y);
                y = static_cast<std::size_t>(parent_[y]);
            }
            std::size_t d = depth[y] == static_cast<std::size_t>(-1) ? 0 : depth[y];
            depth[y] = d;
            for (auto it = path.rbegin(); it != path.rend(); ++it) depth[*it] = ++d;
            best = std::max(best, depth[x]);
        }
        return best;
    }

private:
    std::vector<int> parent_;
    std::vector<std::size_t> size_;  // meaningful at roots only
    std::size_t sets_;
};

// ---------------------------------------------------------------- Task 3 ★★

struct WeightedEdge {
    int u;
    int v;
    long long w;
};

struct SpanningTree {
    long long weight = 0;
    std::vector<WeightedEdge> edges;  // n - (number of components) edges
};

// Kruskal: sort the edges by weight; take an edge if its ends are not yet
// connected (UnionFind), skip it otherwise. For a disconnected graph this
// gives a minimum spanning FOREST.
inline SpanningTree kruskal(std::size_t n, std::vector<WeightedEdge> edges) {
    // TODO
    (void)n;
    (void)edges;
    return {};
}

// Prim from vertex 0: grow one tree; a min-heap of (weight, vertex) of the
// edges leaving it; always take the lightest edge to a vertex not yet in
// the tree. Returns the total weight of a minimum spanning tree of the
// component of vertex 0 (lazy deletion again).
inline long long prim(const WeightedGraph& g) {
    // TODO
    (void)g;
    return -1;
}

// ---------------------------------------------------------------- Task 4 ★★★

// Shortest distances when every weight is 0 or 1, in Theta(n + m) - no
// heap: a std::deque; relaxing a 0-edge pushes to the FRONT, a 1-edge to the
// BACK. (A vertex may be pushed more than once; skip it if popped again with
// a stale distance.) Throws std::invalid_argument if a weight is not 0 or 1.
inline std::vector<long long> zeroOneBfs(const WeightedGraph& g, int s) {
    // TODO
    (void)g;
    (void)s;
    return {};
}

#endif  // SDP_W14_GRAPHS2_H_
