// Your merge sort, quicksort and radix sort against std::sort and
// std::stable_sort, on 32-bit unsigned keys, four kinds of input:
// random, sorted, reversed, and few distinct values (0..9).
// Milliseconds per sort of n keys.
//
//   cmake --preset release
//   cmake --build --preset release --target w12_bench
//   ./build/release/w12_bench            (add --csv for machine-readable output)

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "sorts2.h"

namespace {

using Clock = std::chrono::steady_clock;
using V = std::vector<std::uint32_t>;

V makeInput(const std::string& kind, std::size_t n, std::mt19937& rng) {
    V v(n);
    for (auto& x : v) x = kind == "few" ? static_cast<std::uint32_t>(rng() % 10) : static_cast<std::uint32_t>(rng());
    if (kind == "sorted") std::sort(v.begin(), v.end());
    if (kind == "reversed") std::sort(v.begin(), v.end(), std::greater<>());
    return v;
}

double timeMs(const std::function<void(V&)>& sort, const V& input) {
    double best = 1e300;
    for (int rep = 0; rep < (input.size() <= 100'000 ? 3 : 1); ++rep) {
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
        std::function<void(V&)> fn;
    };
    const std::vector<Algo> algos = {
        {"merge", [](V& v) { mergeSort(v.begin(), v.end()); }},
        {"quick", [](V& v) { quickSort(v.begin(), v.end()); }},
        {"radix", [](V& v) { radixSort(v); }},
        {"std::sort", [](V& v) { std::sort(v.begin(), v.end()); }},
        {"std::stable_sort", [](V& v) { std::stable_sort(v.begin(), v.end()); }},
    };
    const char* kinds[] = {"random", "sorted", "reversed", "few"};

    if (csv) std::printf("n,algorithm,input,ms\n");
    else std::printf("milliseconds per sort of n uint32 keys\n");

    std::mt19937 rng(42);
    for (std::size_t n : {std::size_t{10'000}, std::size_t{100'000}, std::size_t{1'000'000}, std::size_t{10'000'000}}) {
        if (!csv) {
            std::printf("\nn = %zu\n%-17s", n, "");
            for (const char* k : kinds) std::printf(" | %10s", k);
            std::printf("\n");
        }
        V inputs[4];
        for (int k = 0; k < 4; ++k) inputs[k] = makeInput(kinds[k], n, rng);
        for (const Algo& a : algos) {
            if (!csv) std::printf("%-17s", a.name);
            for (int k = 0; k < 4; ++k) {
                const double ms = timeMs(a.fn, inputs[k]);
                if (csv) std::printf("%zu,%s,%s,%.3f\n", n, a.name, kinds[k], ms);
                else std::printf(" | %10.3f", ms);
            }
            if (!csv) std::printf("\n");
        }
    }
    return 0;
}
