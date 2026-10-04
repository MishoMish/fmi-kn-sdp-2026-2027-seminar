// n int keys: insert all, then look all of them up, then look up n keys
// that are NOT there. Average nanoseconds per operation, and the height.
//
// "random" inserts a shuffled 0..n-1; "sorted" inserts 0, 1, ..., n-1
// (only up to n = 20 000: it is Theta(n^2) in total).
//
//   cmake --preset release
//   cmake --build --preset release --target w09_bench
//   ./build/release/w09_bench            (add --csv for machine-readable output)

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <numeric>
#include <random>
#include <set>
#include <vector>

#include "bst.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

template <typename Set>
double run(Set& s, const std::vector<int>& in) {
    const int n = static_cast<int>(in.size());
    long long found = 0;
    const auto start = Clock::now();
    for (int k : in) s.insert(k);
    for (int k : in) found += s.count(k);
    for (int k = n; k < 2 * n; ++k) found += s.count(k);
    const auto stop = Clock::now();
    g_sink = found;
    return std::chrono::duration<double, std::nano>(stop - start).count() / (3.0 * n);
}

// BST has contains(), std::set has count(): one name for both.
struct Bst : BST<int> {
    std::size_t count(int k) const { return contains(k) ? 1 : 0; }
};

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    if (csv) {
        std::printf("n,bst_random,set_random,bst_sorted,set_sorted,height_random,height_sorted\n");
    } else {
        std::printf("ns per operation (insert n, find n present, find n absent), int keys\n\n");
        std::printf("%8s | %10s | %10s | %10s | %10s | %9s | %9s | %8s\n", "n", "BST rand", "set rand",
                    "BST sorted", "set sorted", "h random", "h sorted", "log2 n");
        std::printf(
            "---------+------------+------------+------------+------------+-----------+-----------+---------\n");
    }
    std::mt19937 rng(42);
    for (int n : {1000, 10'000, 20'000, 100'000, 1'000'000}) {
        std::vector<int> sorted(n);
        std::iota(sorted.begin(), sorted.end(), 0);
        std::vector<int> shuffled = sorted;
        std::shuffle(shuffled.begin(), shuffled.end(), rng);

        Bst a;
        const double bstRandom = run(a, shuffled);
        std::set<int> b;
        const double setRandom = run(b, shuffled);
        std::set<int> d;
        const double setSorted = run(d, sorted);

        double bstSorted = -1.0;
        int heightSorted = -1;
        if (n <= 20'000) {
            Bst c;
            bstSorted = run(c, sorted);
            heightSorted = c.height();
        }
        if (csv) {
            std::printf("%d,%.1f,%.1f,%.1f,%.1f,%d,%d\n", n, bstRandom, setRandom, bstSorted, setSorted, a.height(),
                        heightSorted);
        } else {
            char sortedCell[32] = "         -";
            char heightCell[32] = "        -";
            if (bstSorted >= 0) {
                std::snprintf(sortedCell, sizeof sortedCell, "%10.1f", bstSorted);
                std::snprintf(heightCell, sizeof heightCell, "%9d", heightSorted);
            }
            std::printf("%8d | %10.1f | %10.1f | %s | %10.1f | %9d | %s | %8.1f\n", n, bstRandom, setRandom, sortedCell,
                        setSorted, a.height(), heightCell, std::log2(n));
        }
    }
    return 0;
}
