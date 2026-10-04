// Every sort of the week (and std::sort / std::stable_sort) on four kinds
// of input: random, sorted, reversed and nearly sorted (1% of the
// elements swapped with a neighbour). Milliseconds per sort of n ints.
//
// The quadratic sorts run only up to n = 20 000 (at 10^6 bubble sort
// would take about an hour).
//
//   cmake --preset release
//   cmake --build --preset release --target w11_bench
//   ./build/release/w11_bench            (add --csv for machine-readable output)

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "heap.h"
#include "sorts.h"

namespace {

using Clock = std::chrono::steady_clock;
using V = std::vector<int>;
using SortFn = std::function<void(V&)>;

V makeInput(const std::string& kind, int n, std::mt19937& rng) {
    V v(static_cast<std::size_t>(n));
    std::uniform_int_distribution<int> d(0, 1'000'000'000);
    for (int& x : v) x = d(rng);
    if (kind == "random") return v;
    std::sort(v.begin(), v.end());
    if (kind == "reversed") std::reverse(v.begin(), v.end());
    if (kind == "nearly") {
        for (int k = 0; k < n / 100; ++k) {
            const std::size_t i = rng() % static_cast<std::size_t>(n - 1);
            std::swap(v[i], v[i + 1]);
        }
    }
    return v;
}

double timeMs(const SortFn& sort, const V& input) {
    double best = 1e300;
    for (int rep = 0; rep < (input.size() <= 20'000 ? 3 : 1); ++rep) {  // best of 3 for small n
        V v = input;
        const auto start = Clock::now();
        sort(v);
        const auto stop = Clock::now();
        if (!std::is_sorted(v.begin(), v.end())) {
            std::fprintf(stderr, "NOT SORTED\n");
            std::exit(1);
        }
        best = std::min(best, std::chrono::duration<double, std::milli>(stop - start).count());
    }
    return best;
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    struct Algo {
        const char* name;
        bool quadratic;
        SortFn fn;
    };
    const std::vector<Algo> algos = {
        {"selection", true, [](V& v) { selectionSort(v.begin(), v.end()); }},
        {"bubble", true, [](V& v) { bubbleSort(v.begin(), v.end()); }},
        {"shaker", true, [](V& v) { shakerSort(v.begin(), v.end()); }},
        {"insertion", true, [](V& v) { insertionSort(v.begin(), v.end()); }},
        {"shell", false, [](V& v) { shellSort(v.begin(), v.end()); }},
        {"heap", false, [](V& v) { heapSort(v.begin(), v.end()); }},
        {"std::sort", false, [](V& v) { std::sort(v.begin(), v.end()); }},
        {"std::stable_sort", false, [](V& v) { std::stable_sort(v.begin(), v.end()); }},
    };
    const char* kinds[] = {"random", "sorted", "reversed", "nearly"};

    if (csv) std::printf("n,algorithm,input,ms\n");
    else std::printf("milliseconds per sort of n ints\n");

    std::mt19937 rng(42);
    for (int n : {1000, 5000, 20'000, 100'000, 1'000'000}) {
        if (!csv) {
            std::printf("\nn = %d\n%-17s", n, "");
            for (const char* k : kinds) std::printf(" | %10s", k);
            std::printf("\n");
        }
        V inputs[4];
        for (int k = 0; k < 4; ++k) inputs[k] = makeInput(kinds[k], n, rng);
        for (const Algo& a : algos) {
            if (a.quadratic && n > 20'000) continue;
            if (!csv) std::printf("%-17s", a.name);
            for (int k = 0; k < 4; ++k) {
                const double ms = timeMs(a.fn, inputs[k]);
                if (csv) std::printf("%d,%s,%s,%.3f\n", n, a.name, kinds[k], ms);
                else std::printf(" | %10.3f", ms);
            }
            if (!csv) std::printf("\n");
        }
    }
    return 0;
}
