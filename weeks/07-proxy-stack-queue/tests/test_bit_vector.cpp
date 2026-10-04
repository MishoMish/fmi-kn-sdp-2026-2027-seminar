#include <catch_amalgamated.hpp>

#include <stdexcept>
#include <type_traits>
#include <vector>

#include "bit_vector.h"

TEST_CASE("Task 4: read and write single bits", "[task4]") {
    BitVector v(130);  // three 64-bit words
    REQUIRE(v.size() == 130);
    v[0] = true;
    v[63] = true;
    v[64] = true;
    v[129] = true;
    REQUIRE(v[0]);
    REQUIRE_FALSE(v[1]);
    REQUIRE(v[63]);
    REQUIRE(v[64]);
    REQUIRE(v[129]);
    v[63] = false;
    REQUIRE_FALSE(v[63]);
    REQUIRE(v[64]);  // the neighbour in the next word is untouched
    REQUIRE_THROWS_AS(v[130], std::out_of_range);
    const BitVector& view = v;
    REQUIRE(view[129]);
    REQUIRE_THROWS_AS(view[130], std::out_of_range);
}

TEST_CASE("Task 4: v[i] = v[j] copies the bit, not the proxy", "[task4]") {
    BitVector v(10);
    v[2] = true;
    v[7] = v[2];
    REQUIRE(v[7]);
    v[2] = false;
    REQUIRE(v[7]);  // still true: bit 7 got a copy of bit 2's VALUE
    v[2] = v[5];    // false into 2
    REQUIRE_FALSE(v[2]);
}

TEST_CASE("Task 4: flip and count", "[task4]") {
    BitVector v(200);
    for (std::size_t i = 0; i < 200; i += 3) v[i].flip();
    REQUIRE(v.count() == 67);
    v[0].flip();
    REQUIRE_FALSE(v[0]);
    REQUIRE(v.count() == 66);

    BitVector ones(70, true);  // the last word has 6 used bits and 58 unused ones
    REQUIRE(ones.count() == 70);
}

TEST_CASE("Task 4: 8 times less memory than bool, and the proxy trap", "[task4]") {
    const BitVector v(1'000'000);
    REQUIRE(v.bytes() == 125'000);  // a std::vector<char> would need 1'000'000

    BitVector w(4);
    auto r = w[1];  // auto is a BitRef, NOT a bool!
    r = true;       // so this writes into w
    REQUIRE(w[1]);
    STATIC_REQUIRE(!std::is_same<decltype(w[1]), bool>::value);
    bool copy = w[1];  // an explicit bool is a real copy
    w[1] = false;
    REQUIRE(copy);
}
