// Dijkstra two ways, on sparse graphs (m = 4n) and on dense ones (every
// pair of vertices connected):
//   heap  - your dijkstra: a binary heap, Theta((n + m) log n)
//   array - no heap: n times, scan all vertices for the closest unfinished
//           one, Theta(n^2 + m) - the original 1959 version
// Milliseconds per run from vertex 0.
//
//   cmake --preset release
//   cmake --build --preset release --target w14_bench
//   ./build/release/w14_bench            (add --csv for machine-readable output)

#include <chrono>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

#include "graphs2.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

std::vector<long long> dijkstraArray(const WeightedGraph& g, int s) {
    const std::size_t n = g.vertexCount();
    std::vector<long long> dist(n, kInf);
    std::vector<char> done(n, 0);
    dist[static_cast<std::size_t>(s)] = 0;
    for (std::size_t round = 0; round < n; ++round) {
        std::size_t u = n;
        for (std::size_t v = 0; v < n; ++v)  // the closest unfinished vertex: Theta(n)
            if (!done[v] && dist[v] != kInf && (u == n || dist[v] < dist[u])) u = v;
        if (u == n) break;
        done[u] = 1;
        for (const Edge& e : g.neighbors(static_cast<int>(u)))
            if (dist[u] + e.w < dist[static_cast<std::size_t>(e.to)]) dist[static_cast<std::size_t>(e.to)] = dist[u] + e.w;
    }
    return dist;
}

template <typename F>
double timeMs(F f) {
    const auto start = Clock::now();
    const std::vector<long long> d = f();
    const auto stop = Clock::now();
    g_sink = d.empty() ? 0 : d.back();
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    if (csv) std::printf("kind,n,m,algorithm,ms\n");
    else std::printf("%-6s | %9s | %10s | %-5s | %10s\n", "graph", "n", "m", "", "ms");
    std::mt19937 rng(42);
    auto print = [&](const char* kind, std::size_t n, std::size_t m, const char* algo, double ms) {
        if (csv) std::printf("%s,%zu,%zu,%s,%.3f\n", kind, n, m, algo, ms);
        else std::printf("%-6s | %9zu | %10zu | %-5s | %10.3f\n", kind, n, m, algo, ms);
    };
    for (std::size_t n : {std::size_t{1000}, std::size_t{10'000}, std::size_t{100'000}, std::size_t{1'000'000}}) {
        WeightedGraph g(n);
        for (std::size_t e = 0; e < 4 * n; ++e)
            g.addEdge(static_cast<int>(rng() % n), static_cast<int>(rng() % n), static_cast<long long>(rng() % 1000));
        print("sparse", n, 4 * n, "heap", timeMs([&] { return dijkstra(g, 0); }));
        if (n <= 100'000) print("sparse", n, 4 * n, "array", timeMs([&] { return dijkstraArray(g, 0); }));
    }
    for (std::size_t n : {std::size_t{500}, std::size_t{1000}, std::size_t{2000}, std::size_t{4000}}) {
        WeightedGraph g(n);
        for (std::size_t u = 0; u < n; ++u)
            for (std::size_t v = u + 1; v < n; ++v)
                g.addEdge(static_cast<int>(u), static_cast<int>(v), static_cast<long long>(rng() % 1'000'000));
        const std::size_t m = n * (n - 1) / 2;
        print("dense", n, m, "heap", timeMs([&] { return dijkstra(g, 0); }));
        print("dense", n, m, "array", timeMs([&] { return dijkstraArray(g, 0); }));
    }
    return 0;
}
