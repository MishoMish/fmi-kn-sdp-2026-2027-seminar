// Counting the set bits of 10 000 000 random 64-bit numbers, four ways:
//   loop      - test all 64 bits, one by one
//   kernighan - your popcount: x &= x - 1, one step per set bit
//   table     - a 256-entry table, one lookup per byte
//   bitset    - std::bitset<64>::count(): one POPCNT instruction if the
//               compiler may use it (try -march=native), else a library call
// Then your nQueens for n = 8 .. 14.
//
//   cmake --preset release
//   cmake --build --preset release --target w15_bench
//   ./build/release/w15_bench            (add --csv for machine-readable output)

#include <array>
#include <bitset>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

#include "bits.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile unsigned long long g_sink = 0;

unsigned loopCount(u64 x) {
    unsigned c = 0;
    for (int i = 0; i < 64; ++i) c += (x >> i) & 1;
    return c;
}

const std::array<unsigned char, 256> kTable = [] {
    std::array<unsigned char, 256> t{};
    for (int i = 0; i < 256; ++i) t[i] = static_cast<unsigned char>((i & 1) + t[i / 2]);
    return t;
}();

unsigned tableCount(u64 x) {
    unsigned c = 0;
    for (int b = 0; b < 8; ++b) c += kTable[(x >> (8 * b)) & 0xFF];
    return c;
}

template <typename F>
double nsPerNumber(const std::vector<u64>& v, F f) {
    const auto start = Clock::now();
    unsigned long long total = 0;
    for (u64 x : v) total += f(x);
    const auto stop = Clock::now();
    g_sink = total;
    return std::chrono::duration<double, std::nano>(stop - start).count() / static_cast<double>(v.size());
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    std::mt19937_64 rng(42);
    std::vector<u64> v(10'000'000);
    for (u64& x : v) x = rng();

    if (csv) std::printf("what,method,value\n");
    else std::printf("popcount of %zu random 64-bit numbers, ns per number\n", v.size());
    auto row = [&](const char* method, double ns) {
        if (csv) std::printf("popcount,%s,%.3f\n", method, ns);
        else std::printf("  %-10s %7.3f ns\n", method, ns);
    };
    row("loop", nsPerNumber(v, loopCount));
    row("kernighan", nsPerNumber(v, [](u64 x) { return popcount(x); }));
    row("table", nsPerNumber(v, tableCount));
    row("bitset", nsPerNumber(v, [](u64 x) { return static_cast<unsigned>(std::bitset<64>(x).count()); }));

    if (!csv) std::printf("\nnQueens\n");
    for (unsigned n = 8; n <= 14; ++n) {
        const auto start = Clock::now();
        const u64 solutions = nQueens(n);
        const auto stop = Clock::now();
        const double ms = std::chrono::duration<double, std::milli>(stop - start).count();
        if (csv) std::printf("nqueens,%u,%.3f\n", n, ms);
        else std::printf("  n = %2u: %8llu solutions, %9.3f ms\n", n, static_cast<unsigned long long>(solutions), ms);
    }
    return 0;
}
