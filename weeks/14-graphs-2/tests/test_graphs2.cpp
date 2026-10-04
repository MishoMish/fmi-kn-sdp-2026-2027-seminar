#include <catch_amalgamated.hpp>

#include <algorithm>
#include <cmath>
#include <queue>
#include <random>
#include <vector>

#include "graphs2.h"

namespace {

using LV = std::vector<long long>;

//        1 ----4---- 3
//       / \          | \.
//     2/   \1       2|  \6
//     /     \        |   \.
//    0 --5-- 2 --8-- 4 -1- 5
WeightedGraph sample() {
    WeightedGraph g(6);
    g.addEdge(0, 1, 2);
    g.addEdge(0, 2, 5);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 4);
    g.addEdge(2, 4, 8);
    g.addEdge(3, 4, 2);
    g.addEdge(3, 5, 6);
    g.addEdge(4, 5, 1);
    return g;
}

std::vector<WeightedEdge> sampleEdges() {
    return {{0, 1, 2}, {0, 2, 5}, {1, 2, 1}, {1, 3, 4}, {2, 4, 8}, {3, 4, 2}, {3, 5, 6}, {4, 5, 1}};
}

struct Random {
    WeightedGraph g;
    std::vector<WeightedEdge> edges;
};

Random randomGraph(std::size_t n, std::size_t m, unsigned seed, bool directed, long long maxW = 100) {
    std::mt19937 rng(seed);
    Random r{WeightedGraph(n, directed), {}};
    for (std::size_t i = 0; i < m; ++i) {
        const int u = static_cast<int>(rng() % n), v = static_cast<int>(rng() % n);
        const long long w = static_cast<long long>(rng() % static_cast<unsigned>(maxW + 1));
        r.g.addEdge(u, v, w);
        r.edges.push_back({u, v, w});
    }
    return r;
}

// Bellman-Ford: slow and simple, for checking.
LV bellmanFord(std::size_t n, const std::vector<WeightedEdge>& edges, int s, bool directed) {
    LV d(n, kInf);
    d[static_cast<std::size_t>(s)] = 0;
    for (std::size_t round = 0; round + 1 < n; ++round) {
        bool changed = false;
        for (const auto& e : edges) {
            if (d[e.u] != kInf && d[e.u] + e.w < d[e.v]) d[e.v] = d[e.u] + e.w, changed = true;
            if (!directed && d[e.v] != kInf && d[e.v] + e.w < d[e.u]) d[e.u] = d[e.v] + e.w, changed = true;
        }
        if (!changed) break;
    }
    return d;
}

// O(n^2) Prim on a matrix, for checking.
long long slowMstWeight(std::size_t n, const std::vector<WeightedEdge>& edges) {
    std::vector<LV> w(n, LV(n, kInf));
    for (const auto& e : edges) {
        if (e.u == e.v) continue;
        w[e.u][e.v] = std::min(w[e.u][e.v], e.w);
        w[e.v][e.u] = std::min(w[e.v][e.u], e.w);
    }
    std::vector<char> in(n, 0);
    LV best(n, kInf);
    long long total = 0;
    for (std::size_t start = 0; start < n; ++start) {  // every component
        if (in[start]) continue;
        best[start] = 0;
        while (true) {
            std::size_t u = n;
            for (std::size_t v = 0; v < n; ++v)
                if (!in[v] && best[v] != kInf && (u == n || best[v] < best[u])) u = v;
            if (u == n) break;
            in[u] = 1;
            total += best[u];
            for (std::size_t v = 0; v < n; ++v)
                if (!in[v] && w[u][v] < best[v]) best[v] = w[u][v];
        }
    }
    return total;
}

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: dijkstra on the sample", "[task1]") {
    REQUIRE(dijkstra(sample(), 0) == LV{0, 2, 3, 6, 8, 9});
    REQUIRE(dijkstra(sample(), 5) == LV{9, 7, 8, 3, 1, 0});
}

TEST_CASE("Task 1: dijkstra - a longer path can be shorter", "[task1]") {
    WeightedGraph g(4, true);
    g.addEdge(0, 3, 10);  // direct: 10
    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(2, 3, 1);   // three hops: 3
    REQUIRE(dijkstra(g, 0) == LV{0, 1, 2, 3});
    REQUIRE(dijkstra(g, 3) == LV{kInf, kInf, kInf, 0});  // directed: nothing leaves 3
}

TEST_CASE("Task 1: dijkstra against Bellman-Ford", "[task1]") {
    for (unsigned seed = 0; seed < 30; ++seed) {
        const bool directed = seed % 2 == 0;
        const Random r = randomGraph(150, 600, seed, directed);
        const LV d = dijkstra(r.g, 0);
        REQUIRE(d.size() == 150);
        REQUIRE(d == bellmanFord(150, r.edges, 0, directed));
    }
}

TEST_CASE("Task 1: dijkstra with zero weights and parallel edges", "[task1]") {
    WeightedGraph g(3);
    g.addEdge(0, 1, 7);
    g.addEdge(0, 1, 3);  // a parallel, cheaper edge
    g.addEdge(1, 2, 0);
    g.addEdge(2, 2, 5);  // a self-loop
    REQUIRE(dijkstra(g, 0) == LV{0, 3, 3});
}

TEST_CASE("Task 1: dijkstraPath", "[task1]") {
    REQUIRE(dijkstraPath(sample(), 0, 5) == std::vector<int>{0, 1, 3, 4, 5});
    REQUIRE(dijkstraPath(sample(), 2, 2) == std::vector<int>{2});
    WeightedGraph g(3, true);
    g.addEdge(0, 1, 1);
    REQUIRE(dijkstraPath(g, 0, 2).empty());
    REQUIRE(dijkstraPath(g, 1, 0).empty());

    const Random r = randomGraph(300, 1500, 77, false);
    const LV d = dijkstra(r.g, 0);
    for (int t = 0; t < 300; t += 13) {
        const std::vector<int> p = dijkstraPath(r.g, 0, t);
        if (d[t] == kInf) {
            REQUIRE(p.empty());
            continue;
        }
        REQUIRE(p.front() == 0);
        REQUIRE(p.back() == t);
        long long len = 0;  // the path's own length must be the distance
        for (std::size_t i = 0; i + 1 < p.size(); ++i) {
            long long best = kInf;
            for (const Edge& e : r.g.neighbors(p[i]))
                if (e.to == p[i + 1]) best = std::min(best, e.w);
            REQUIRE(best != kInf);  // a real edge
            len += best;
        }
        REQUIRE(len == d[t]);
    }
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: UnionFind basics", "[task2]") {
    UnionFind uf(6);
    REQUIRE(uf.setCount() == 6);
    REQUIRE(uf.unite(0, 1));
    REQUIRE(uf.unite(2, 3));
    REQUIRE(uf.unite(1, 3));
    REQUIRE_FALSE(uf.unite(0, 2));  // already together
    REQUIRE(uf.setCount() == 3);
    REQUIRE(uf.connected(0, 3));
    REQUIRE_FALSE(uf.connected(0, 4));
    REQUIRE(uf.setSize(2) == 4);
    REQUIRE(uf.setSize(5) == 1);
}

TEST_CASE("Task 2: UnionFind against a naive labelling", "[task2]") {
    std::mt19937 rng(3);
    constexpr int n = 500;
    UnionFind uf(n);
    std::vector<int> label(n);
    for (int i = 0; i < n; ++i) label[i] = i;
    for (int step = 0; step < 3000; ++step) {
        const int a = static_cast<int>(rng() % n), b = static_cast<int>(rng() % n);
        if (rng() % 2) {
            const bool merged = label[a] != label[b];
            REQUIRE(uf.unite(a, b) == merged);
            if (merged) {
                const int old = label[b];
                for (int& l : label)
                    if (l == old) l = label[a];
            }
        } else {
            REQUIRE(uf.connected(a, b) == (label[a] == label[b]));
        }
    }
}

TEST_CASE("Task 2: union by size keeps the trees shallow", "[task2]") {
    constexpr int n = 1 << 16;
    UnionFind chain(n);
    for (int i = 0; i + 1 < n; ++i) chain.unite(i, i + 1);  // naive: a chain of length n
    REQUIRE(chain.setCount() == 1);
    REQUIRE(chain.height() <= 16);  // log2 n

    UnionFind pairs(n);  // pairs, then pairs of pairs, ...: the worst case for union by size
    for (int step = 1; step < n; step *= 2)
        for (int i = 0; i + step < n; i += 2 * step) pairs.unite(i, i + step);
    REQUIRE(pairs.setCount() == 1);
    REQUIRE(pairs.height() <= 16);
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: kruskal on the sample", "[task3]") {
    const SpanningTree t = kruskal(6, sampleEdges());
    REQUIRE(t.weight == 1 + 1 + 2 + 2 + 4);  // 1-2, 4-5, 0-1, 3-4, 1-3
    REQUIRE(t.edges.size() == 5);
    UnionFind uf(6);
    for (const auto& e : t.edges) REQUIRE(uf.unite(e.u, e.v));  // no cycles
    REQUIRE(uf.setCount() == 1);                                // spans everything
}

TEST_CASE("Task 3: kruskal on random graphs, and forests", "[task3]") {
    for (unsigned seed = 0; seed < 20; ++seed) {
        const Random r = randomGraph(120, 200 + seed * 20, seed, false);
        const SpanningTree t = kruskal(120, r.edges);
        REQUIRE(t.weight == slowMstWeight(120, r.edges));
        UnionFind components(120);
        for (const auto& e : r.edges) components.unite(e.u, e.v);
        REQUIRE(t.edges.size() == 120 - components.setCount());
    }
    REQUIRE(kruskal(4, {}).edges.empty());
}

TEST_CASE("Task 3: prim agrees with kruskal", "[task3]") {
    REQUIRE(prim(sample()) == 10);
    for (unsigned seed = 0; seed < 20; ++seed) {
        Random r = randomGraph(150, 400, 100 + seed, false);
        for (int i = 0; i + 1 < 150; ++i) {  // make it connected with heavy edges
            r.g.addEdge(i, i + 1, 1000);
            r.edges.push_back({i, i + 1, 1000});
        }
        REQUIRE(prim(r.g) == kruskal(150, r.edges).weight);
    }
}

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: zeroOneBfs", "[task4]") {
    WeightedGraph g(5, true);
    g.addEdge(0, 1, 1);
    g.addEdge(0, 2, 0);
    g.addEdge(2, 1, 0);
    g.addEdge(1, 3, 1);
    g.addEdge(3, 4, 0);
    REQUIRE(zeroOneBfs(g, 0) == LV{0, 0, 0, 1, 1});

    for (unsigned seed = 0; seed < 20; ++seed) {
        const Random r = randomGraph(300, 1200, seed, seed % 2 == 1, 1);
        REQUIRE(zeroOneBfs(r.g, 0) == dijkstra(r.g, 0));
    }
    WeightedGraph bad(2);
    bad.addEdge(0, 1, 2);
    REQUIRE_THROWS_AS(zeroOneBfs(bad, 0), std::invalid_argument);
}
