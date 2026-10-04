// Time to pushBack n ints, for different growth strategies.
//
//   cmake --preset release
//   cmake --build --preset release --target w04_bench
//   ./build/release/w04_bench            (add --csv for machine-readable output)
//
// "doubling" and "doubling+reserve" are your DynamicArray from starter/.

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include "dynamic_array.h"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long g_sink = 0;

// A deliberately bad dynamic array: grows by a constant amount.
class AdditiveArray {
public:
    explicit AdditiveArray(std::size_t step) : step_(step) {}
    void pushBack(int v) {
        if (size_ == capacity_) {
            std::unique_ptr<int[]> fresh(new int[capacity_ + step_]);
            std::copy(data_.get(), data_.get() + size_, fresh.get());
            data_ = std::move(fresh);
            capacity_ += step_;
        }
        data_[size_++] = v;
    }
    int last() const { return data_[size_ - 1]; }

private:
    std::unique_ptr<int[]> data_;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    std::size_t step_;
};

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
    if (csv) {
        std::printf("n,doubling_ms,reserve_ms,std_vector_ms,plus64_ms\n");
    } else {
        std::printf("Time to pushBack n ints (best of several runs)\n\n");
        std::printf("%9s | %12s | %16s | %13s | %10s\n", "n", "doubling ms", "doubling+reserve",
                    "std::vector", "+64 ms");
        std::printf("----------+--------------+------------------+---------------+-----------\n");
    }
    for (std::size_t n = std::size_t{1} << 10; n <= (std::size_t{1} << 22); n *= 2) {
        const int reps = n <= (std::size_t{1} << 16) ? 7 : 3;
        const int count = static_cast<int>(n);
        const double doubling = millis([&] {
            DynamicArray<int> a;
            for (int i = 0; i < count; ++i) a.pushBack(i);
            g_sink = a.size() ? a.back() : 0;
        }, reps);
        const double reserved = millis([&] {
            DynamicArray<int> a;
            a.reserve(n);
            for (int i = 0; i < count; ++i) a.pushBack(i);
            g_sink = a.size() ? a.back() : 0;
        }, reps);
        const double vec = millis([&] {
            std::vector<int> a;
            for (int i = 0; i < count; ++i) a.push_back(i);
            g_sink = a.back();
        }, reps);
        double additive = -1.0;
        if (n <= (std::size_t{1} << 18)) {
            additive = millis([&] {
                AdditiveArray a(64);
                for (int i = 0; i < count; ++i) a.pushBack(i);
                g_sink = a.last();
            }, n <= (std::size_t{1} << 16) ? 3 : 1);
        }
        if (csv) {
            std::printf("%zu,%.4f,%.4f,%.4f,%.4f\n", n, doubling, reserved, vec, additive);
        } else if (additive < 0) {
            std::printf("%9zu | %12.3f | %16.3f | %13.3f | %10s\n", n, doubling, reserved, vec, "-");
        } else {
            std::printf("%9zu | %12.3f | %16.3f | %13.3f | %10.2f\n", n, doubling, reserved, vec, additive);
        }
    }
    if (!csv) {
        std::printf("\nDoubling: x2 n -> ~x2 time (amortized O(1) per push).\n"
                    "+64:      x2 n -> ~x4 time (O(n) per push on average).\n"
                    "If 'doubling' prints ~0, Task 1 is not implemented yet.\n");
    }
    return 0;
}
