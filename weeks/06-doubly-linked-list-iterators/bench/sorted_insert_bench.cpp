// Stroustrup's experiment: keep a sequence sorted while inserting n random
// numbers. For each number: walk from the front to the first larger
// element (linear search - the only option for a list), then insert there.
// Both steps are Theta(n), so the whole thing is Theta(n^2) for both
// containers. Which one is faster?
//
//   cmake --preset release
//   cmake --build --preset release --target w06_bench
//   ./build/release/w06_bench            (add --csv for machine-readable output)

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <list>
#include <random>
#include <vector>

#include "list.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

template <typename Container>
double sortedInsertMillis(const std::vector<int>& values) {
    const auto start = Clock::now();
    Container c;
    for (int v : values) {
        auto it = c.begin();
        while (it != c.end() && *it < v) {
            ++it;
        }
        c.insert(it, v);
    }
    const auto stop = Clock::now();
    g_sink = static_cast<long long>(c.size());
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

// Insert m values at one fixed position in the middle - the position is
// already known, so the list does no searching at all.
double middleInsertList(std::size_t n, std::size_t m) {
    List<int> l;
    for (std::size_t i = 0; i < n; ++i) l.pushBack(static_cast<int>(i));
    auto mid = l.begin();
    std::advance(mid, static_cast<long>(n / 2));
    const auto start = Clock::now();
    for (std::size_t i = 0; i < m; ++i) l.insert(mid, static_cast<int>(i));
    const auto stop = Clock::now();
    g_sink = static_cast<long long>(l.size());
    return std::chrono::duration<double, std::nano>(stop - start).count() / static_cast<double>(m);
}

double middleInsertVector(std::size_t n, std::size_t m) {
    std::vector<int> v(n);
    const auto start = Clock::now();
    for (std::size_t i = 0; i < m; ++i) {
        v.insert(v.begin() + static_cast<long>(v.size() / 2), static_cast<int>(i));
    }
    const auto stop = Clock::now();
    g_sink = static_cast<long long>(v.size());
    return std::chrono::duration<double, std::nano>(stop - start).count() / static_cast<double>(m);
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    std::mt19937 rng(2026);

    if (csv) {
        std::printf("part,n,vector,your_list,std_list\n");
    } else {
        std::printf("Part 1: insert n random ints keeping the sequence sorted (ms)\n\n");
        std::printf("%7s | %10s | %10s | %10s\n", "n", "vector", "your List", "std::list");
        std::printf("--------+------------+------------+-----------\n");
    }
    for (std::size_t n = 1000; n <= 32000; n *= 2) {
        std::vector<int> values(n);
        for (int& v : values) v = static_cast<int>(rng() % 1'000'000);
        const double vec = sortedInsertMillis<std::vector<int>>(values);
        const double mine = sortedInsertMillis<List<int>>(values);
        const double stdl = sortedInsertMillis<std::list<int>>(values);
        if (csv) {
            std::printf("sorted,%zu,%.3f,%.3f,%.3f\n", n, vec, mine, stdl);
        } else {
            std::printf("%7zu | %10.2f | %10.2f | %10.2f\n", n, vec, mine, stdl);
        }
    }

    if (!csv) {
        std::printf("\nPart 2: insert at a KNOWN middle position (ns per insertion, 1000 insertions)\n\n");
        std::printf("%9s | %10s | %10s\n", "size", "vector", "your List");
        std::printf("----------+------------+-----------\n");
    }
    for (std::size_t n = 1000; n <= 1'000'000; n *= 10) {
        const double vec = middleInsertVector(n, 1000);
        const double mine = middleInsertList(n, 1000);
        if (csv) {
            std::printf("middle,%zu,%.3f,%.3f,-1\n", n, vec, mine);
        } else {
            std::printf("%9zu | %10.1f | %10.1f\n", n, vec, mine);
        }
    }
    if (!csv) {
        std::printf("\nWhen the position must be FOUND, the search dominates - and the vector\n"
                    "searches much faster. When the position is already KNOWN, the list wins.\n");
    }
    return 0;
}
