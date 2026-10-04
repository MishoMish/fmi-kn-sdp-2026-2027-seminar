// One BFS from vertex 0 over three representations of the same random
// undirected graph with n vertices and 4n edges (average degree 8):
//   matrix - n x n bytes; neighbours of u = scan the whole row (Theta(n))
//   lists  - your Graph (a vector of vectors) and your bfsDistances
//   csr    - "compressed sparse row": ALL neighbours in one array, vertex u's
//            are targets[offset[u] .. offset[u+1]) - one allocation, no gaps
// Milliseconds per BFS and the memory each representation takes.
//
//   cmake --preset release
//   cmake --build --preset release --target w13_bench
//   ./build/release/w13_bench            (add --csv for machine-readable output)

#include <chrono>
#include <cstdio>
#include <cstring>
#include <queue>
#include <random>
#include <vector>

#include "graph.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

std::vector<int> bfsMatrix(const std::vector<char>& m, std::size_t n, int s) {
    std::vector<int> dist(n, -1);
    std::queue<int> q;
    dist[s] = 0;
    q.push(s);
    while (!q.empty()) {
        const int u = q.front();
        q.pop();
        const char* row = &m[static_cast<std::size_t>(u) * n];
        for (std::size_t v = 0; v < n; ++v) {
            if (row[v] && dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(static_cast<int>(v));
            }
        }
    }
    return dist;
}

struct Csr {
    std::vector<int> offset;   // n + 1
    std::vector<int> targets;  // 2m for an undirected graph
};

Csr toCsr(const Graph& g) {
    Csr c;
    c.offset.assign(g.vertexCount() + 1, 0);
    for (std::size_t u = 0; u < g.vertexCount(); ++u)
        c.offset[u + 1] = c.offset[u] + static_cast<int>(g.neighbors(static_cast<int>(u)).size());
    c.targets.reserve(static_cast<std::size_t>(c.offset.back()));
    for (std::size_t u = 0; u < g.vertexCount(); ++u)
        for (int v : g.neighbors(static_cast<int>(u))) c.targets.push_back(v);
    return c;
}

std::vector<int> bfsCsr(const Csr& c, int s) {
    const std::size_t n = c.offset.size() - 1;
    std::vector<int> dist(n, -1);
    std::vector<int> queue;  // a vector as the queue: BFS never needs to shrink it
    queue.reserve(n);
    dist[s] = 0;
    queue.push_back(s);
    for (std::size_t head = 0; head < queue.size(); ++head) {
        const int u = queue[head];
        for (int k = c.offset[u]; k < c.offset[u + 1]; ++k) {
            const int v = c.targets[k];
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                queue.push_back(v);
            }
        }
    }
    return dist;
}

template <typename F>
double timeMs(F f) {
    double best = 1e300;
    for (int rep = 0; rep < 3; ++rep) {
        const auto start = Clock::now();
        const std::vector<int> d = f();
        const auto stop = Clock::now();
        g_sink = d.empty() ? 0 : d.back();  // empty: the starter is not done yet
        best = std::min(best, std::chrono::duration<double, std::milli>(stop - start).count());
    }
    return best;
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    if (csv) {
        std::printf("n,structure,ms,mb\n");
    } else {
        std::printf("one BFS, random graph with n vertices and 4n edges\n\n");
        std::printf("%9s | %-7s | %10s | %9s\n", "n", "", "ms", "MB");
        std::printf("----------+---------+------------+----------\n");
    }
    std::mt19937 rng(42);
    for (std::size_t n : {std::size_t{1000}, std::size_t{4000}, std::size_t{16'000}, std::size_t{100'000},
                          std::size_t{1'000'000}, std::size_t{4'000'000}}) {
        Graph g(n);
        for (std::size_t e = 0; e < 4 * n; ++e)
            g.addEdge(static_cast<int>(rng() % n), static_cast<int>(rng() % n));

        auto print = [&](const char* name, double ms, double mb) {
            if (csv) std::printf("%zu,%s,%.3f,%.1f\n", n, name, ms, mb);
            else std::printf("%9zu | %-7s | %10.3f | %9.1f\n", n, name, ms, mb);
        };
        if (n <= 16'000) {
            std::vector<char> m(n * n, 0);
            for (std::size_t u = 0; u < n; ++u)
                for (int v : g.neighbors(static_cast<int>(u))) m[u * n + static_cast<std::size_t>(v)] = 1;
            print("matrix", timeMs([&] { return bfsMatrix(m, n, 0); }), static_cast<double>(n * n) / 1e6);
        }
        std::size_t listBytes = n * sizeof(std::vector<int>);
        for (std::size_t u = 0; u < n; ++u) listBytes += g.neighbors(static_cast<int>(u)).capacity() * sizeof(int);
        print("lists", timeMs([&] { return bfsDistances(g, 0); }), static_cast<double>(listBytes) / 1e6);
        const Csr c = toCsr(g);
        print("csr", timeMs([&] { return bfsCsr(c, 0); }),
              static_cast<double>((c.offset.size() + c.targets.size()) * sizeof(int)) / 1e6);
        if (!csv) std::printf("----------+---------+------------+----------\n");
    }
    return 0;
}
