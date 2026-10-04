#include <catch_amalgamated.hpp>

#include <algorithm>
#include <bitset>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "bits.h"

namespace {

unsigned slowPopcount(u64 x) { return static_cast<unsigned>(std::bitset<64>(x).count()); }

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: toBinary and fromBinary", "[task1]") {
    REQUIRE(toBinary(0) == "0");
    REQUIRE(toBinary(1) == "1");
    REQUIRE(toBinary(11) == "1011");
    REQUIRE(toBinary(255) == "11111111");
    REQUIRE(toBinary(~u64{0}) == std::string(64, '1'));
    REQUIRE(fromBinary("1011") == 11);
    REQUIRE(fromBinary("0001") == 1);
    REQUIRE(fromBinary(std::string(64, '1')) == ~u64{0});
    REQUIRE_THROWS_AS(fromBinary(""), std::invalid_argument);
    REQUIRE_THROWS_AS(fromBinary("102"), std::invalid_argument);
    REQUIRE_THROWS_AS(fromBinary("1" + std::string(64, '0')), std::invalid_argument);  // 65 bits

    std::mt19937_64 rng(1);
    for (int i = 0; i < 1000; ++i) {
        const u64 x = rng() >> (rng() % 64);
        REQUIRE(fromBinary(toBinary(x)) == x);
        REQUIRE(toBinary(x) == (x == 0 ? "0" : std::bitset<64>(x).to_string().substr(64 - toBinary(x).size())));
    }
}

TEST_CASE("Task 1: get, set, clear, toggle", "[task1]") {
    const u32 x = 0b1010;
    REQUIRE(getBit(x, 1));
    REQUIRE_FALSE(getBit(x, 0));
    REQUIRE(setBit(x, 0) == 0b1011);
    REQUIRE(setBit(x, 1) == x);       // already set
    REQUIRE(clearBit(x, 3) == 0b0010);
    REQUIRE(clearBit(x, 2) == x);     // already clear
    REQUIRE(toggleBit(x, 0) == 0b1011);
    REQUIRE(toggleBit(x, 1) == 0b1000);
    REQUIRE(setBit(0, 31) == 0x80000000u);
    REQUIRE(getBit(0x80000000u, 31));
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: popcount", "[task2]") {
    REQUIRE(popcount(0) == 0);
    REQUIRE(popcount(1) == 1);
    REQUIRE(popcount(0b1011) == 3);
    REQUIRE(popcount(~u64{0}) == 64);
    std::mt19937_64 rng(2);
    for (int i = 0; i < 10'000; ++i) {
        const u64 x = rng() & rng();
        REQUIRE(popcount(x) == slowPopcount(x));
    }
}

TEST_CASE("Task 2: isPowerOfTwo and countTrailingZeros", "[task2]") {
    REQUIRE_FALSE(isPowerOfTwo(0));
    for (unsigned i = 0; i < 64; ++i) {
        const u64 p = u64{1} << i;
        REQUIRE(isPowerOfTwo(p));
        REQUIRE(countTrailingZeros(p) == i);
        if (i > 1) REQUIRE_FALSE(isPowerOfTwo(p + 1));
        if (i > 1) REQUIRE_FALSE(isPowerOfTwo(p - 1));
    }
    REQUIRE(countTrailingZeros(0) == 64);
    REQUIRE(countTrailingZeros(6) == 1);
    REQUIRE(countTrailingZeros(0b101000) == 3);
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: nextPowerOfTwo", "[task3]") {
    REQUIRE(nextPowerOfTwo(0) == 1);
    REQUIRE(nextPowerOfTwo(1) == 1);
    REQUIRE(nextPowerOfTwo(2) == 2);
    REQUIRE(nextPowerOfTwo(3) == 4);
    REQUIRE(nextPowerOfTwo(5) == 8);
    REQUIRE(nextPowerOfTwo(1000) == 1024);
    REQUIRE(nextPowerOfTwo(1024) == 1024);
    REQUIRE(nextPowerOfTwo(1025) == 2048);
    REQUIRE(nextPowerOfTwo(u64{1} << 63) == u64{1} << 63);
    REQUIRE_THROWS_AS(nextPowerOfTwo((u64{1} << 63) + 1), std::overflow_error);
    std::mt19937_64 rng(3);
    for (int i = 0; i < 1000; ++i) {
        const u64 x = (rng() >> (1 + rng() % 63)) + 1;
        const u64 p = nextPowerOfTwo(x);
        REQUIRE(isPowerOfTwo(p));
        REQUIRE(p >= x);
        REQUIRE(p / 2 < x);
    }
}

TEST_CASE("Task 3: reverseBits", "[task3]") {
    REQUIRE(reverseBits(0) == 0);
    REQUIRE(reverseBits(1) == 0x80000000u);
    REQUIRE(reverseBits(0x80000000u) == 1);
    REQUIRE(reverseBits(0b1011) == 0xD0000000u);
    REQUIRE(reverseBits(0xFFFFFFFFu) == 0xFFFFFFFFu);
    std::mt19937 rng(4);
    for (int i = 0; i < 1000; ++i) {
        const u32 x = rng();
        REQUIRE(reverseBits(reverseBits(x)) == x);
        for (unsigned b = 0; b < 32; ++b) REQUIRE(getBit(reverseBits(x), 31 - b) == getBit(x, b));
    }
}

TEST_CASE("Task 3: findUnique", "[task3]") {
    REQUIRE(findUnique({7}) == 7);
    REQUIRE(findUnique({4, 1, 2, 1, 2}) == 4);
    REQUIRE(findUnique({-3, 5, 5}) == -3);
    std::mt19937 rng(5);
    std::vector<int> v;
    for (int i = 0; i < 100'000; ++i) {
        const int x = static_cast<int>(rng());
        v.push_back(x);
        v.push_back(x);
    }
    v.push_back(123456);
    std::shuffle(v.begin(), v.end(), rng);
    REQUIRE(findUnique(v) == 123456);
}

// ---------------------------------------------------------------- Task 4

TEST_CASE("Task 4: allSubsets", "[task4]") {
    const auto s = allSubsets({1, 2, 3});
    REQUIRE(s == std::vector<std::vector<int>>{{}, {1}, {2}, {1, 2}, {3}, {1, 3}, {2, 3}, {1, 2, 3}});
    REQUIRE(allSubsets({}) == std::vector<std::vector<int>>{{}});
    const auto big = allSubsets({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
    REQUIRE(big.size() == 4096);
    std::set<std::vector<int>> distinct(big.begin(), big.end());
    REQUIRE(distinct.size() == 4096);
}

TEST_CASE("Task 4: grayCode", "[task4]") {
    REQUIRE(grayCode(0) == std::vector<u32>{0});
    REQUIRE(grayCode(1) == std::vector<u32>{0, 1});
    REQUIRE(grayCode(3) == std::vector<u32>{0b000, 0b001, 0b011, 0b010, 0b110, 0b111, 0b101, 0b100});
    for (unsigned n = 1; n <= 16; ++n) {
        const auto g = grayCode(n);
        REQUIRE(g.size() == (std::size_t{1} << n));
        std::vector<char> seen(g.size(), 0);
        for (std::size_t i = 0; i < g.size(); ++i) {
            REQUIRE(g[i] < g.size());
            REQUIRE_FALSE(seen[g[i]]);  // every n-bit number exactly once
            seen[g[i]] = 1;
            REQUIRE(popcount(g[i] ^ g[(i + 1) % g.size()]) == 1);  // neighbours differ in one bit
        }
    }
}

// ---------------------------------------------------------------- Task 5

TEST_CASE("Task 5: nQueens", "[task5]") {
    const std::vector<u64> known{1, 0, 0, 2, 10, 4, 40, 92, 352, 724, 2680, 14200};  // n = 1..12
    for (unsigned n = 1; n <= 12; ++n) {
        INFO("n = " << n);
        REQUIRE(nQueens(n) == known[n - 1]);
    }
}
