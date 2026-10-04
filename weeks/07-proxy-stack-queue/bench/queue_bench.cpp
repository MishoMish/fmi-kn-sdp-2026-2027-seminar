// Five FIFO queues under the same load: keep n elements in the queue and
// do 2'000'000 (pop + push) pairs. Time per pair, in nanoseconds.
//
//   cmake --preset release
//   cmake --build --preset release --target w07_bench
//   ./build/release/w07_bench            (add --csv for machine-readable output)

#include <chrono>
#include <cstdio>
#include <cstring>
#include <deque>
#include <list>
#include <queue>
#include <vector>

#include "ring_queue.h"
#include "two_stack_queue.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;
constexpr int kPairs = 2'000'000;

// The "obvious" wrong queue: pop from the front of a vector shifts everything.
struct VectorQueue {
    std::vector<int> v;
    void push(int x) { v.push_back(x); }
    int& front() { return v.front(); }
    void pop() { v.erase(v.begin()); }
};

template <typename Q>
double nsPerPair(std::size_t n, int pairs) {
    Q q;
    for (std::size_t i = 0; i < n; ++i) q.push(static_cast<int>(i));
    long long sum = 0;
    const auto start = Clock::now();
    for (int i = 0; i < pairs; ++i) {
        sum += q.front();
        q.pop();
        q.push(i);
    }
    const auto stop = Clock::now();
    g_sink = sum;
    return std::chrono::duration<double, std::nano>(stop - start).count() / pairs;
}

}  // namespace

int main(int argc, char** argv) {
    const bool csv = argc > 1 && std::strcmp(argv[1], "--csv") == 0;
    if (csv) {
        std::printf("n,ring,std_deque,std_list,two_stacks,vector_erase\n");
    } else {
        std::printf("ns per (pop + push), queue kept at n elements\n\n");
        std::printf("%9s | %8s | %10s | %10s | %10s | %13s\n", "n", "ring", "std::deque", "std::list",
                    "two stacks", "vector erase");
        std::printf("----------+----------+------------+------------+------------+--------------\n");
    }
    for (std::size_t n : {std::size_t{10}, std::size_t{1000}, std::size_t{100'000}, std::size_t{1'000'000}}) {
        const double ring = nsPerPair<RingQueue<int>>(n, kPairs);
        const double dq = nsPerPair<std::queue<int, std::deque<int>>>(n, kPairs);
        const double ls = nsPerPair<std::queue<int, std::list<int>>>(n, kPairs);
        const double two = nsPerPair<TwoStackQueue<int>>(n, kPairs);
        double vec = -1.0;
        if (n <= 100'000) {
            vec = nsPerPair<VectorQueue>(n, n <= 1000 ? kPairs : 20'000);
        }
        if (csv) {
            std::printf("%zu,%.3f,%.3f,%.3f,%.3f,%.3f\n", n, ring, dq, ls, two, vec);
        } else if (vec < 0) {
            std::printf("%9zu | %8.2f | %10.2f | %10.2f | %10.2f | %13s\n", n, ring, dq, ls, two, "-");
        } else {
            std::printf("%9zu | %8.2f | %10.2f | %10.2f | %10.2f | %13.1f\n", n, ring, dq, ls, two, vec);
        }
    }
    if (!csv) {
        std::printf("\nAll but the last are Theta(1) per operation; the vector shifts n elements on every pop.\n"
                    "If 'ring' prints ~0 or crashes, Task 2 is not implemented yet.\n");
    }
    return 0;
}
