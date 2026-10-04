// Locality experiments for week 02. Same number of operations,
// very different running times - only the memory access pattern changes.
//
// Build with the release preset:
//   cmake --preset release
//   cmake --build --preset release --target w02_bench
//   ./build/release/w02_bench            (add --csv for machine-readable output)
//
// Parts 1 and 2 use your code from starter/; parts 3 and 4 are self-contained.

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <list>
#include <numeric>
#include <random>
#include <vector>

#include "matrix.h"
#include "traversal.h"

namespace {

using Clock = std::chrono::steady_clock;

volatile std::int64_t g_sink = 0;  // keeps results "used" so nothing is optimized away
volatile double g_sinkD = 0.0;

template <typename F>
double medianMillis(F&& f, int repeats = 5) {
    std::vector<double> samples;
    for (int r = 0; r < repeats; ++r) {
        const auto start = Clock::now();
        f();
        const auto stop = Clock::now();
        samples.push_back(std::chrono::duration<double, std::milli>(stop - start).count());
    }
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}

bool g_csv = false;

void header(const char* title) {
    if (!g_csv) {
        std::printf("\n== %s\n", title);
    }
}

// ---------------------------------------------------------------- part 1
void traversalOrder() {
    header("Part 1: sum of a 4096 x 4096 matrix of doubles (128 MB), two loop orders");
    // double, not int: floating-point addition is not associative, so the
    // compiler may not swap the two loops for us (with integers GCC -O3
    // does exactly that - "loop interchange" - and hides the effect).
    const std::size_t n = 4096;
    Matrix<double> m(n, n);
    for (std::size_t r = 0; r < n; ++r) {
        for (std::size_t c = 0; c < n; ++c) {
            m(r, c) = static_cast<double>(r ^ c);
        }
    }
    const double row = medianMillis([&] { g_sinkD = sumRowMajor(m); });
    const double col = medianMillis([&] { g_sinkD = sumColMajor(m); }, 3);
    if (g_csv) {
        std::printf("traversal,row_major,%.3f\ntraversal,col_major,%.3f\n", row, col);
    } else {
        std::printf("  row-major (sequential):       %9.2f ms\n", row);
        std::printf("  column-major (stride %zu B): %9.2f ms   x%.1f slower\n",
                    n * sizeof(double), col, col / row);
    }
}

// ---------------------------------------------------------------- part 2
void transposeKinds() {
    header("Part 2: transpose of a 2048 x 2048 matrix of 64-bit ints (32 MB)");
    const std::size_t n = 2048;
    Matrix<std::int64_t> m(n, n);
    for (std::size_t i = 0; i < m.size(); ++i) {
        m.data()[i] = static_cast<std::int64_t>(i);
    }
    const double naive = medianMillis([&] { g_sink = transpose(m)(1, 0); }, 3);
    const double blocked = medianMillis([&] { g_sink = transposeBlocked(m, 32)(1, 0); }, 3);
    if (g_csv) {
        std::printf("transpose,naive,%.3f\ntranspose,blocked32,%.3f\n", naive, blocked);
    } else {
        std::printf("  naive:          %9.2f ms\n", naive);
        std::printf("  blocked (32):   %9.2f ms   x%.1f faster\n", blocked, naive / blocked);
    }
}

// ---------------------------------------------------------------- part 3
void strideSweep() {
    header("Part 3: touch all 16M ints of a 64 MB array with a growing stride");
    const std::size_t n = std::size_t{1} << 24;
    std::vector<std::int32_t> a(n, 1);
    if (!g_csv) {
        std::printf("  %6s %8s  %s\n", "stride", "bytes", "ns per access");
    }
    for (std::size_t stride = 1; stride <= 256; stride *= 2) {
        // Every element is visited exactly once, whatever the stride:
        // offset 0, stride, 2*stride, ..., then offset 1, 1 + stride, ...
        const double ms = medianMillis([&] {
            std::int64_t sum = 0;
            for (std::size_t start = 0; start < stride; ++start) {
                for (std::size_t i = start; i < n; i += stride) {
                    sum += a[i];
                }
            }
            g_sink = sum;
        }, 3);
        const double ns = ms * 1e6 / static_cast<double>(n);
        if (g_csv) {
            std::printf("stride,%zu,%.4f\n", stride, ns);
        } else {
            std::printf("  %6zu %8zu  %6.2f\n", stride, stride * sizeof(std::int32_t), ns);
        }
    }
}

// ---------------------------------------------------------------- part 4
struct Node {
    std::int64_t value;
    Node* next;
};

void pointerChasing() {
    header("Part 4: sum of 4M values in three containers");
    const std::size_t n = std::size_t{1} << 22;

    std::vector<std::int64_t> vec(n);
    std::iota(vec.begin(), vec.end(), 0);
    std::list<std::int64_t> lst(vec.begin(), vec.end());

    // Linked nodes in a random memory order: every step is a cache miss.
    std::vector<Node> pool(n);
    std::vector<std::size_t> order(n);
    std::iota(order.begin(), order.end(), 0);
    std::shuffle(order.begin(), order.end(), std::mt19937(2026));
    for (std::size_t k = 0; k < n; ++k) {
        pool[order[k]].value = static_cast<std::int64_t>(k);
        pool[order[k]].next = k + 1 < n ? &pool[order[k + 1]] : nullptr;
    }
    Node* head = &pool[order[0]];

    const double tVec = medianMillis([&] {
        g_sink = std::accumulate(vec.begin(), vec.end(), std::int64_t{0});
    });
    const double tList = medianMillis([&] {
        g_sink = std::accumulate(lst.begin(), lst.end(), std::int64_t{0});
    });
    const double tShuffled = medianMillis([&] {
        std::int64_t sum = 0;
        for (Node* p = head; p != nullptr; p = p->next) {
            sum += p->value;
        }
        g_sink = sum;
    }, 3);

    const auto perElem = [n](double ms) { return ms * 1e6 / static_cast<double>(n); };
    if (g_csv) {
        std::printf("chasing,vector,%.4f\nchasing,list,%.4f\nchasing,shuffled_nodes,%.4f\n",
                    perElem(tVec), perElem(tList), perElem(tShuffled));
    } else {
        std::printf("  std::vector:              %7.2f ms  (%5.2f ns / element)\n", tVec, perElem(tVec));
        std::printf("  std::list (fresh):        %7.2f ms  (%5.2f ns / element)\n", tList, perElem(tList));
        std::printf("  linked nodes, shuffled:   %7.2f ms  (%5.2f ns / element)\n", tShuffled,
                    perElem(tShuffled));
        std::printf("  All three are Theta(n). The difference is the memory access pattern.\n");
    }
}

}  // namespace

int main(int argc, char** argv) {
    g_csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    if (g_csv) {
        std::printf("part,variant,value\n");
    }
    traversalOrder();
    transposeKinds();
    strideSweep();
    pointerChasing();
    if (!g_csv) {
        std::printf("\nIf parts 1-2 print ~0 ms, Tasks 2 and 4 are not implemented yet.\n");
    }
    return 0;
}
