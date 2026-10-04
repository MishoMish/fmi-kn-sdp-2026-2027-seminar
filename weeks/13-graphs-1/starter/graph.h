#ifndef SDP_W13_GRAPH_H_
#define SDP_W13_GRAPH_H_

#include <cstddef>
#include <functional>
#include <optional>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// A graph with vertices 0 .. n-1, stored as ADJACENCY LISTS: for every
// vertex, the vector of its neighbours, in the order the edges were added.
// Directed: addEdge(u, v) adds u -> v only. Undirected: both directions.
//
// Every algorithm below visits neighbours in that order - the tests
// depend on it (e.g. the exact DFS order).
class Graph {
public:
    explicit Graph(std::size_t n, bool directed = false) : adj_(n), directed_(directed) {}

    // ------------------------------------------------------------ given

    std::size_t vertexCount() const noexcept { return adj_.size(); }
    std::size_t edgeCount() const noexcept { return edges_; }
    bool directed() const noexcept { return directed_; }

    void addEdge(int u, int v) {
        check(u);
        check(v);
        adj_[u].push_back(v);
        if (!directed_ && u != v) adj_[v].push_back(u);
        ++edges_;
    }

    const std::vector<int>& neighbors(int u) const {
        check(u);
        return adj_[u];
    }

    std::size_t degree(int u) const { return neighbors(u).size(); }  // out-degree if directed

    static Graph fromEdges(std::size_t n, const std::vector<std::pair<int, int>>& edges, bool directed = false) {
        Graph g(n, directed);
        for (auto [u, v] : edges) g.addEdge(u, v);
        return g;
    }

private:
    void check(int u) const {
        if (u < 0 || static_cast<std::size_t>(u) >= adj_.size()) throw std::out_of_range("Graph: no such vertex");
    }

    std::vector<std::vector<int>> adj_;
    std::size_t edges_ = 0;
    bool directed_;
};

// ---------------------------------------------------------------- Task 1 ★

// matrix[u][v] == 1 iff there is an edge u -> v (both ways if undirected).
// n^2 cells, whatever the number of edges.
inline std::vector<std::vector<char>> adjacencyMatrix(const Graph& g) {
    // TODO
    (void)g;
    return {};
}

// In-degree of every vertex of a directed graph: how many edges point TO it.
inline std::vector<std::size_t> inDegrees(const Graph& g) {
    // TODO
    (void)g;
    return {};
}

// The reverse (transpose) of a directed graph: every u -> v becomes v -> u.
// Add the reversed edges in the order you meet them: u = 0, 1, ...; for each
// u its neighbours in order.
inline Graph reversed(const Graph& g) {
    // TODO
    return Graph(g.vertexCount(), true);
}

// ---------------------------------------------------------------- Task 2 ★

// Breadth-first search from s: dist[v] = number of edges on a shortest path
// s -> v, or -1 if v is unreachable. A queue; mark a vertex when you PUSH it.
inline std::vector<int> bfsDistances(const Graph& g, int s) {
    // TODO
    (void)g;
    (void)s;
    return {};
}

// A shortest path s -> t as a list of vertices (s first, t last), or an
// empty vector if t is unreachable. BFS that remembers parent[v] - the
// vertex v was discovered from - then walk back from t.
inline std::vector<int> shortestPath(const Graph& g, int s, int t) {
    // TODO
    (void)g;
    (void)s;
    (void)t;
    return {};
}

// ---------------------------------------------------------------- Task 3 ★★

// A maze: '#' is a wall, anything else is free; 'S' and 'T' mark the start
// and the target. Moves: up, down, left, right. The length (number of moves)
// of a shortest route S -> T, or -1. The graph is IMPLICIT: never build
// adjacency lists - the neighbours of (r, c) are computed on the fly.
inline int gridShortestPath(const std::vector<std::string>& grid) {
    // TODO
    (void)grid;
    return -2;
}

// ---------------------------------------------------------------- Task 4 ★

// Depth-first search from s, recursively: the vertices in the order they are
// FIRST visited (pre-order). Neighbours in adjacency-list order.
inline std::vector<int> dfsOrder(const Graph& g, int s) {
    // TODO (a recursive helper with a visited vector)
    (void)g;
    (void)s;
    return {};
}

// Connected components of an undirected graph: comp[v] = 0, 1, 2, ... -
// the component of vertex 0 is 0, then the component of the smallest vertex
// not yet labelled is 1, and so on. Any traversal (BFS or DFS) per component.
inline std::vector<int> connectedComponents(const Graph& g) {
    // TODO
    (void)g;
    return {};
}

// ---------------------------------------------------------------- Task 5 ★★

// Does a DIRECTED graph have a cycle? DFS with three colours:
//   white - not visited, grey - on the current DFS path, black - finished.
// An edge to a GREY vertex closes a cycle.
inline bool hasCycle(const Graph& g) {
    // TODO
    (void)g;
    return false;
}

// A topological order of a directed graph - every edge u -> v has u before v
// - or std::nullopt if there is a cycle. Kahn's algorithm: queue the vertices
// of in-degree 0 (smallest first), pop one, "remove" its out-edges, queue
// the new in-degree-0 vertices. (Or: DFS, reverse finishing order.)
inline std::optional<std::vector<int>> topologicalSort(const Graph& g) {
    // TODO
    (void)g;
    return std::nullopt;
}

// ---------------------------------------------------------------- Task 6 ★★

// Can the vertices of an undirected graph be coloured 0/1 so that every edge
// joins different colours? BFS from every uncoloured vertex: colour it 0,
// neighbours get the opposite colour; a neighbour with the SAME colour means
// an odd cycle - not bipartite. On success fill colour[v].
inline bool isBipartite(const Graph& g, std::vector<int>& colour) {
    // TODO
    (void)g;
    colour.clear();
    return false;
}

// ---------------------------------------------------------------- Task 7 ★★★

// The SAME order as dfsOrder, without recursion: an explicit stack of
// (vertex, index of the next neighbour to try). The tests compare it with
// dfsOrder and run it on a path of 1 000 000 vertices.
inline std::vector<int> dfsOrderIterative(const Graph& g, int s) {
    // TODO
    (void)g;
    (void)s;
    return {};
}

#endif  // SDP_W13_GRAPH_H_
