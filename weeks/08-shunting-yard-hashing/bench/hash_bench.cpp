// n string keys: insert all, then look all of them up, then look up n keys
// that are NOT there. Average nanoseconds per operation.
//
//   cmake --preset release
//   cmake --build --preset release --target w08_bench
//   ./build/release/w08_bench            (add --csv for machine-readable output)

#include <chrono>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "chained_hash_map.h"
#include "hashing.h"
#include "probing_hash_set.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

struct SumHash {
    std::size_t operator()(const std::string& s) const { return static_cast<std::size_t>(sumHash(s)); }
};
struct Fnv {
    std::size_t operator()(const std::string& s) const { return static_cast<std::size_t>(fnv1a(s)); }
};

std::vector<std::string> keys(std::size_t n, const char* prefix) {
    std::vector<std::string> out;
    out.reserve(n);
    for (std::size_t i = 0; i < n; ++i) out.push_back(prefix + std::to_string(i));
    return out;
}

// Runs insert / hit / miss with the three callables; ns per operation.
template <typename Insert, typename Find>
double run(const std::vector<std::string>& in, const std::vector<std::string>& out, Insert insert, Find find) {
    long long found = 0;
    const auto start = Clock::now();
    for (const auto& k : in) insert(k);
    for (const auto& k : in) found += find(k);
    for (const auto& k : out) found += find(k);
    const auto stop = Clock::now();
    g_sink = found;
    return std::chrono::duration<double, std::nano>(stop - start).count() / (3.0 * in.size());
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    if (csv) {
        std::printf("n,chained_std,chained_fnv,chained_sum,probing,unordered_map,map\n");
    } else {
        std::printf("ns per operation (insert n, find n present, find n absent), string keys\n\n");
        std::printf("%8s | %11s | %11s | %11s | %8s | %13s | %8s\n", "n", "chain std", "chain FNV", "chain SUM",
                    "probing", "unordered_map", "std::map");
        std::printf("---------+-------------+-------------+-------------+----------+---------------+---------\n");
    }
    for (std::size_t n : {std::size_t{1000}, std::size_t{10'000}, std::size_t{100'000}, std::size_t{1'000'000}}) {
        const auto in = keys(n, "key");
        const auto out = keys(n, "nokey");
        ChainedHashMap<std::string, int> a;
        const double chainStd = run(in, out, [&](const std::string& k) { a.insert(k, 1); },
                                    [&](const std::string& k) { return a.contains(k) ? 1 : 0; });
        ChainedHashMap<std::string, int, Fnv> b;
        const double chainFnv = run(in, out, [&](const std::string& k) { b.insert(k, 1); },
                                    [&](const std::string& k) { return b.contains(k) ? 1 : 0; });
        double chainSum = -1.0;
        if (n <= 10'000) {
            ChainedHashMap<std::string, int, SumHash> c;
            chainSum = run(in, out, [&](const std::string& k) { c.insert(k, 1); },
                           [&](const std::string& k) { return c.contains(k) ? 1 : 0; });
        }
        ProbingHashSet<std::string> p;
        const double probing = run(in, out, [&](const std::string& k) { p.insert(k); },
                                   [&](const std::string& k) { return p.contains(k) ? 1 : 0; });
        std::unordered_map<std::string, int> u;
        const double um = run(in, out, [&](const std::string& k) { u.emplace(k, 1); },
                              [&](const std::string& k) { return static_cast<int>(u.count(k)); });
        std::map<std::string, int> t;
        const double tm = run(in, out, [&](const std::string& k) { t.emplace(k, 1); },
                              [&](const std::string& k) { return static_cast<int>(t.count(k)); });
        if (csv) {
            std::printf("%zu,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", n, chainStd, chainFnv, chainSum, probing, um, tm);
        } else if (chainSum < 0) {
            std::printf("%8zu | %11.1f | %11.1f | %11s | %8.1f | %13.1f | %8.1f\n", n, chainStd, chainFnv, "-",
                        probing, um, tm);
        } else {
            std::printf("%8zu | %11.1f | %11.1f | %11.1f | %8.1f | %13.1f | %8.1f\n", n, chainStd, chainFnv,
                        chainSum, probing, um, tm);
        }
    }
    if (!csv) {
        std::printf("\nSUM is the sum of the characters: \"key123\" and \"key321\" collide. std::map is a\n"
                    "balanced tree (weeks 09-10): Theta(log n) comparisons of strings per operation.\n");
    }
    return 0;
}
