// Where a linked list wins and where it loses.
//
//   cmake --preset release
//   cmake --build --preset release --target w05_bench
//   ./build/release/w05_bench            (add --csv for machine-readable output)

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <deque>
#include <numeric>
#include <vector>

#include "singly_linked_list.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

template <typename F>
double millis(F&& f, int repeats) {
    double best = 1e300;
    for (int r = 0; r < repeats; ++r) {
        const auto start = Clock::now();
        f();
        const auto stop = Clock::now();
        best = std::min(best, std::chrono::duration<double, std::milli>(stop - start).count());
    }
    return best;
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;

    // Part 1: insert n elements at the FRONT.
    if (csv) {
        std::printf("part,n,list_ms,vector_ms,deque_ms\n");
    } else {
        std::printf("Part 1: n insertions at the front\n\n");
        std::printf("%9s | %10s | %22s | %10s\n", "n", "list ms", "vector insert(begin) ms", "deque ms");
        std::printf("----------+------------+-------------------------+-----------\n");
    }
    for (std::size_t n = std::size_t{1} << 10; n <= (std::size_t{1} << 20); n *= 2) {
        const int count = static_cast<int>(n);
        const double list = millis([&] {
            SinglyLinkedList<int> l;
            for (int i = 0; i < count; ++i) l.pushFront(i);
            g_sink = static_cast<long long>(l.size());
        }, 3);
        double vec = -1.0;
        if (n <= (std::size_t{1} << 17)) {
            vec = millis([&] {
                std::vector<int> v;
                for (int i = 0; i < count; ++i) v.insert(v.begin(), i);
                g_sink = v.front();
            }, n <= (std::size_t{1} << 15) ? 3 : 1);
        }
        const double dq = millis([&] {
            std::deque<int> d;
            for (int i = 0; i < count; ++i) d.push_front(i);
            g_sink = d.front();
        }, 3);
        if (csv) {
            std::printf("front,%zu,%.4f,%.4f,%.4f\n", n, list, vec, dq);
        } else if (vec < 0) {
            std::printf("%9zu | %10.3f | %23s | %10.3f\n", n, list, "-", dq);
        } else {
            std::printf("%9zu | %10.3f | %23.3f | %10.3f\n", n, list, vec, dq);
        }
    }

    // Part 2: sum all elements.
    if (!csv) {
        std::printf("\nPart 2: sum of n elements (ns per element)\n\n");
        std::printf("%9s | %8s | %8s\n", "n", "list", "vector");
        std::printf("----------+----------+---------\n");
    }
    for (std::size_t n = std::size_t{1} << 10; n <= (std::size_t{1} << 22); n *= 4) {
        SinglyLinkedList<int> l;
        std::vector<int> v(n);
        std::iota(v.begin(), v.end(), 0);
        // Build the list in a scattered order: many push/pop at the front
        // leave the allocator's free memory fragmented, as in a long-running
        // program.
        for (std::size_t i = 0; i < n; ++i) {
            l.pushBack(static_cast<int>(i));
            if (i % 3 == 0) {
                l.pushFront(-1);
                l.popFront();
            }
        }
        const double tl = millis([&] {
            long long s = 0;
            l.forEach([&](int x) { s += x; });
            g_sink = s;
        }, 5);
        const double tv = millis([&] {
            g_sink = std::accumulate(v.begin(), v.end(), 0LL);
        }, 5);
        const double nl = tl * 1e6 / static_cast<double>(n);
        const double nv = tv * 1e6 / static_cast<double>(n);
        if (csv) {
            std::printf("sum,%zu,%.4f,%.4f,-1\n", n, nl, nv);
        } else {
            std::printf("%9zu | %8.2f | %8.2f\n", n, nl, nv);
        }
    }
    if (!csv) {
        std::printf("\nIf 'list' prints ~0, Task 1 is not implemented yet.\n");
    }
    return 0;
}
