// Linear search vs binary search, as the array grows.
//
//   cmake --preset release
//   cmake --build --preset release --target w03_bench
//   ./build/release/w03_bench            (add --csv for machine-readable output)
//
// "yours" is lowerBound from starter/binary_search.h.

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <numeric>
#include <random>
#include <vector>

#include "binary_search.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

// Average nanoseconds per query of `search` over `queries`.
template <typename F>
double nsPerQuery(const std::vector<int>& queries, F&& search) {
    long long found = 0;
    const auto start = Clock::now();
    for (int q : queries) {
        found += search(q);
    }
    const auto stop = Clock::now();
    g_sink = found;
    return std::chrono::duration<double, std::nano>(stop - start).count() /
           static_cast<double>(queries.size());
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    std::mt19937 rng(2026);

    if (csv) {
        std::printf("n,linear_ns,yours_ns,std_ns\n");
    } else {
        std::printf("Average time per search, sorted std::vector<int> of n even numbers\n\n");
        std::printf("%10s | %12s | %12s | %12s\n", "n", "linear ns", "yours ns", "std ns");
        std::printf("-----------+--------------+--------------+-------------\n");
    }

    for (std::size_t n = std::size_t{1} << 4; n <= (std::size_t{1} << 24); n *= 4) {
        std::vector<int> v(n);
        for (std::size_t i = 0; i < n; ++i) {
            v[i] = static_cast<int>(2 * i);  // sorted; odd queries are misses
        }
        std::uniform_int_distribution<int> pick(0, static_cast<int>(2 * n));
        std::vector<int> queries(200000);
        for (int& q : queries) {
            q = pick(rng);
        }

        double linear = -1.0;
        if (n <= (std::size_t{1} << 14)) {
            const std::vector<int> few(queries.begin(), queries.begin() + 20000);
            linear = nsPerQuery(few, [&](int q) {
                return std::find(v.begin(), v.end(), q) != v.end();
            });
        }
        const double yours = nsPerQuery(queries, [&](int q) {
            return contains(v.begin(), v.end(), q) ? 1 : 0;
        });
        const double stdlib = nsPerQuery(queries, [&](int q) {
            return std::binary_search(v.begin(), v.end(), q) ? 1 : 0;
        });

        if (csv) {
            std::printf("%zu,%.3f,%.3f,%.3f\n", n, linear, yours, stdlib);
        } else if (linear < 0) {
            std::printf("%10zu | %12s | %12.1f | %12.1f\n", n, "-", yours, stdlib);
        } else {
            std::printf("%10zu | %12.1f | %12.1f | %12.1f\n", n, linear, yours, stdlib);
        }
    }

    if (!csv) {
        std::printf("\nn grows x4 per row: linear search gets ~x4 slower, binary search only adds\n"
                    "~2 steps. At large n each step is a cache miss - see week 02.\n"
                    "If 'yours' prints ~0, Task 4 is not implemented yet.\n");
    }
    return 0;
}
