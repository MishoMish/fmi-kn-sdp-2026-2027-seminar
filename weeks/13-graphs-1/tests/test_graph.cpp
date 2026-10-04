#include <catch_amalgamated.hpp>

#include <algorithm>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "graph.h"

namespace {

using V = std::vector<int>;

//   0 --- 1          5 --- 6        7
//   |     |
//   2 --- 3 --- 4
Graph sample() { return Graph::fromEdges(8, {{0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}, {5, 6}}); }

// Course prerequisites (directed): an edge u -> v means "u before v".
//   0 intro programming -> 1 OOP -> 2 data structures -> 4 algorithms
//   3 discrete maths --------------^
Graph courses() { return Graph::fromEdges(5, {{0, 1}, {1, 2}, {3, 2}, {2, 4}}, true); }

Graph randomGraph(std::size_t n, std::size_t m, unsigned seed, bool directed) {
    std::mt19937 rng(seed);
    Graph g(n, directed);
    for (std::size_t i = 0; i < m; ++i)
        g.addEdge(static_cast<int>(rng() % n), static_cast<int>(rng() % n));
    return g;
}

bool isTopological(const Graph& g, const V& order) {
    if (order.size() != g.vertexCount()) return false;
    V pos(g.vertexCount(), -1);
    for (std::size_t i = 0; i < order.size(); ++i) {
        if (order[i] < 0 || static_cast<std::size_t>(order[i]) >= g.vertexCount() || pos[order[i]] != -1) return false;
        pos[order[i]] = static_cast<int>(i);
    }
    for (std::size_t u = 0; u < g.vertexCount(); ++u)
        for (int v : g.neighbors(static_cast<int>(u)))
            if (pos[u] >= pos[v]) return false;
    return true;
}

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: adjacencyMatrix", "[task1]") {
    const auto m = adjacencyMatrix(sample());
    REQUIRE(m.size() == 8);
    REQUIRE(m[0][1] == 1);
    REQUIRE(m[1][0] == 1);  // undirected: symmetric
    REQUIRE(m[0][3] == 0);
    REQUIRE(m[7][7] == 0);
    int ones = 0;
    for (const auto& row : m) ones += static_cast<int>(std::count(row.begin(), row.end(), 1));
    REQUIRE(ones == 2 * 6);

    const auto d = adjacencyMatrix(courses());
    REQUIRE(d[0][1] == 1);
    REQUIRE(d[1][0] == 0);  // directed: not symmetric
}

TEST_CASE("Task 1: inDegrees and reversed", "[task1]") {
    const Graph g = courses();
    REQUIRE(inDegrees(g) == std::vector<std::size_t>{0, 1, 2, 0, 1});
    const Graph r = reversed(g);
    REQUIRE(r.directed());
    REQUIRE(r.edgeCount() == 4);
    REQUIRE(r.neighbors(2) == V{1, 3});
    REQUIRE(r.neighbors(4) == V{2});
    REQUIRE(r.neighbors(0).empty());
    REQUIRE(inDegrees(r) == std::vector<std::size_t>{1, 1, 1, 1, 0});
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: bfsDistances", "[task2]") {
    REQUIRE(bfsDistances(sample(), 0) == V{0, 1, 1, 2, 3, -1, -1, -1});
    REQUIRE(bfsDistances(sample(), 6) == V{-1, -1, -1, -1, -1, 1, 0, -1});
    REQUIRE(bfsDistances(courses(), 3) == V{-1, -1, 1, 0, 2});  // directed: follow the arrows
}

TEST_CASE("Task 2: bfsDistances are exactly the shortest distances", "[task2]") {
    for (unsigned seed = 0; seed < 20; ++seed) {
        const Graph g = randomGraph(300, 450, seed, false);
        const V d = bfsDistances(g, 0);
        REQUIRE(d.size() == g.vertexCount());
        REQUIRE(d[0] == 0);
        for (std::size_t u = 0; u < g.vertexCount(); ++u) {
            bool hasParent = u == 0;
            for (int v : g.neighbors(static_cast<int>(u))) {
                if (d[u] >= 0) REQUIRE(d[v] >= 0);           // reachable spreads along edges
                if (d[u] >= 0) REQUIRE(d[v] <= d[u] + 1);    // no edge skips a level
                if (d[u] > 0 && d[v] == d[u] - 1) hasParent = true;
            }
            if (d[u] > 0) REQUIRE(hasParent);                // every level comes from the previous
        }
    }
}

TEST_CASE("Task 2: shortestPath", "[task2]") {
    REQUIRE(shortestPath(sample(), 0, 4) == V{0, 1, 3, 4});
    REQUIRE(shortestPath(sample(), 4, 4) == V{4});
    REQUIRE(shortestPath(sample(), 0, 6).empty());
    REQUIRE(shortestPath(courses(), 0, 4) == V{0, 1, 2, 4});
    REQUIRE(shortestPath(courses(), 4, 0).empty());  // arrows go the other way

    const Graph g = randomGraph(500, 900, 99, false);
    const V d = bfsDistances(g, 0);
    REQUIRE(d.size() == 500);
    for (int t = 0; t < 500; t += 17) {
        const V p = shortestPath(g, 0, t);
        if (d[t] < 0) {
            REQUIRE(p.empty());
            continue;
        }
        REQUIRE(p.size() == static_cast<std::size_t>(d[t]) + 1);
        REQUIRE(p.front() == 0);
        REQUIRE(p.back() == t);
        for (std::size_t i = 0; i + 1 < p.size(); ++i) {
            const auto& nb = g.neighbors(p[i]);
            REQUIRE(std::find(nb.begin(), nb.end(), p[i + 1]) != nb.end());  // real edges
        }
    }
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: gridShortestPath", "[task3]") {
    REQUIRE(gridShortestPath({"S.T"}) == 2);
    REQUIRE(gridShortestPath({"S#T"}) == -1);
    REQUIRE(gridShortestPath({
                "S.#.....",
                ".##.###.",
                "....#T#.",
                ".##.#.#.",
                "....#...",
            }) == 19);
    REQUIRE(gridShortestPath({"S..", "...", "..."}) == -1);  // no T
    REQUIRE(gridShortestPath({"T#S"}) == -1);
    REQUIRE(gridShortestPath({"ST"}) == 1);
}

TEST_CASE("Task 3: gridShortestPath on a 1000 x 1000 open grid", "[task3]") {
    std::vector<std::string> grid(1000, std::string(1000, '.'));
    grid[0][0] = 'S';
    grid[999][999] = 'T';
    REQUIRE(gridShortestPath(grid) == 1998);
    for (int r = 1; r < 1000; r += 2) {  // a serpentine of walls
        std::fill(grid[r].begin(), grid[r].end(), '#');
        grid[r][(r / 2) % 2 == 0 ? 999 : 0] = '.';
    }
    grid[999][999] = 'T';
    REQUIRE(gridShortestPath(grid) == 499'500);  // snakes through every corridor
}

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: dfsOrder", "[task4]") {
    REQUIRE(dfsOrder(sample(), 0) == V{0, 1, 3, 2, 4});
    REQUIRE(dfsOrder(sample(), 4) == V{4, 3, 1, 0, 2});
    REQUIRE(dfsOrder(sample(), 7) == V{7});
    REQUIRE(dfsOrder(courses(), 0) == V{0, 1, 2, 4});
}

TEST_CASE("Task 4: connectedComponents", "[task4]") {
    REQUIRE(connectedComponents(sample()) == V{0, 0, 0, 0, 0, 1, 1, 2});
    REQUIRE(connectedComponents(Graph(3)) == V{0, 1, 2});

    // a long path: no recursion depth problem with BFS (or iterative DFS)
    Graph path(200'000);
    for (int i = 0; i + 1 < 200'000; ++i) path.addEdge(i, i + 1);
    const V comp = connectedComponents(path);
    REQUIRE(std::all_of(comp.begin(), comp.end(), [](int c) { return c == 0; }));
}

// ---------------------------------------------------------------- Task 5

TEST_CASE("Task 5: hasCycle", "[task5]") {
    REQUIRE_FALSE(hasCycle(courses()));
    Graph g = courses();
    g.addEdge(4, 1);  // algorithms before OOP?
    REQUIRE(hasCycle(g));
    REQUIRE(hasCycle(Graph::fromEdges(1, {{0, 0}}, true)));  // a self-loop
    // two paths to the same vertex are NOT a cycle (that is why grey != black)
    REQUIRE_FALSE(hasCycle(Graph::fromEdges(4, {{0, 1}, {0, 2}, {1, 3}, {2, 3}}, true)));
    REQUIRE(hasCycle(Graph::fromEdges(4, {{0, 1}, {2, 3}, {3, 2}}, true)));  // not reachable from 0
}

TEST_CASE("Task 5: topologicalSort", "[task5]") {
    const auto order = topologicalSort(courses());
    REQUIRE(order.has_value());
    REQUIRE(isTopological(courses(), *order));

    Graph g = courses();
    g.addEdge(4, 1);
    REQUIRE_FALSE(topologicalSort(g).has_value());

    std::mt19937 rng(5);
    for (int trial = 0; trial < 20; ++trial) {  // random DAGs: edges only from lower to higher label...
        const std::size_t n = 200;
        V label(n);
        std::iota(label.begin(), label.end(), 0);
        std::shuffle(label.begin(), label.end(), rng);  // ...under a random relabelling
        Graph dag(n, true);
        for (int e = 0; e < 600; ++e) {
            std::size_t a = rng() % n, b = rng() % n;
            if (a == b) continue;
            if (a > b) std::swap(a, b);
            dag.addEdge(label[a], label[b]);
        }
        const auto o = topologicalSort(dag);
        REQUIRE(o.has_value());
        REQUIRE(isTopological(dag, *o));
        REQUIRE_FALSE(hasCycle(dag));
    }
}

// ---------------------------------------------------------------- Task 6

TEST_CASE("Task 6: isBipartite", "[task6]") {
    std::vector<int> colour;
    const Graph even = Graph::fromEdges(6, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 0}});
    REQUIRE(isBipartite(even, colour));
    REQUIRE(colour.size() == 6);
    for (std::size_t u = 0; u < 6; ++u)
        for (int v : even.neighbors(static_cast<int>(u))) REQUIRE(colour[u] != colour[v]);

    const Graph odd = Graph::fromEdges(5, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}});
    REQUIRE_FALSE(isBipartite(odd, colour));

    REQUIRE(isBipartite(sample(), colour));  // the square 0-1-3-2 is even
    Graph two = Graph::fromEdges(7, {{0, 1}, {2, 3}, {3, 4}, {4, 2}});  // a triangle in a second component
    REQUIRE_FALSE(isBipartite(two, colour));
    REQUIRE_FALSE(isBipartite(Graph::fromEdges(1, {{0, 0}}), colour));  // a self-loop
}

// ---------------------------------------------------------------- Task 7

TEST_CASE("Task 7: dfsOrderIterative matches dfsOrder", "[task7]") {
    REQUIRE(dfsOrderIterative(sample(), 0) == V{0, 1, 3, 2, 4});
    for (unsigned seed = 0; seed < 30; ++seed) {
        const Graph g = randomGraph(200, 300 + seed * 10, seed, seed % 2 == 0);
        for (int s : {0, 17, 199}) REQUIRE(dfsOrderIterative(g, s) == dfsOrder(g, s));
    }
}

TEST_CASE("Task 7: dfsOrderIterative on a 1 000 000-vertex path", "[task7]") {
    constexpr int n = 1'000'000;
    Graph path(n, true);
    for (int i = 0; i + 1 < n; ++i) path.addEdge(i, i + 1);
    V expected(n);
    std::iota(expected.begin(), expected.end(), 0);
    REQUIRE(dfsOrderIterative(path, 0) == expected);
}
