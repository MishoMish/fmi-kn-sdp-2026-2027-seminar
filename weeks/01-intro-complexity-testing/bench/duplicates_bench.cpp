// Task 3: the doubling experiment.
//
// Times the three hasDuplicate* functions on inputs with NO duplicates
// (the worst case: nobody can stop early), doubling n each row.
// Build with the "release" preset, otherwise you measure the debugger,
// not the algorithm:
//
//   cmake --preset release
//   cmake --build --preset release --target w01_bench
//   ./build/release/w01_bench            (add --csv for machine-readable output)

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <numeric>
#include <random>
#include <vector>

#include "duplicates.h"

namespace {

using Clock = std::chrono::steady_clock;

// Prevents the compiler from deleting a call whose result is unused.
volatile bool g_sink = false;

template <typename F>
double medianMillis(F&& f, int repeats) {
    std::vector<double> samples;
    for (int r = 0; r < repeats; ++r) {
        const auto start = Clock::now();
        g_sink = f();
        const auto stop = Clock::now();
        samples.push_back(std::chrono::duration<double, std::milli>(stop - start).count());
    }
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    const std::size_t kNaiveLimit = std::size_t{1} << 15;  // n^2 gets slow beyond this

    std::mt19937 rng(2026);
    double prev[3] = {0.0, 0.0, 0.0};

    if (csv) {
        std::printf("n,naive_ms,sorting_ms,hashing_ms\n");
    } else {
        std::printf("Doubling experiment: inputs with no duplicates (worst case)\n\n");
        std::printf("%9s | %11s %6s | %11s %6s | %11s %6s\n",
                    "n", "naive ms", "ratio", "sorting ms", "ratio", "hashing ms", "ratio");
        std::printf("----------+--------------------+--------------------+-------------------\n");
    }

    for (std::size_t n = std::size_t{1} << 10; n <= (std::size_t{1} << 20); n *= 2) {
        std::vector<int> values(n);
        std::iota(values.begin(), values.end(), 0);
        std::shuffle(values.begin(), values.end(), rng);

        double t[3] = {-1.0, -1.0, -1.0};
        if (n <= kNaiveLimit) {
            t[0] = medianMillis([&] { return hasDuplicateNaive(values); }, 3);
        }
        t[1] = medianMillis([&] { return hasDuplicateSorting(values); }, 5);
        t[2] = medianMillis([&] { return hasDuplicateHashing(values); }, 5);

        if (csv) {
            std::printf("%zu,%.4f,%.4f,%.4f\n", n, t[0], t[1], t[2]);
        } else {
            std::printf("%9zu", n);
            for (int k = 0; k < 3; ++k) {
                if (t[k] < 0) {
                    std::printf(" | %11s %6s", "-", "");
                } else if (prev[k] > 0) {
                    std::printf(" | %11.3f %6.2f", t[k], t[k] / prev[k]);
                } else {
                    std::printf(" | %11.3f %6s", t[k], "");
                }
            }
            std::printf("\n");
        }
        for (int k = 0; k < 3; ++k) {
            prev[k] = t[k];
        }
    }

    if (!csv) {
        std::printf("\nratio = time(n) / time(n/2). Theta(n) -> ~2, Theta(n log n) -> a bit "
                    "above 2, Theta(n^2) -> ~4.\n");
        std::printf("If every time is close to 0, Task 2 is not implemented yet.\n");
    }
    return 0;
}
