#include <catch_amalgamated.hpp>

#include <set>
#include <string>

#include "hashing.h"

TEST_CASE("Task 4: fnv1a matches the published test vectors", "[task4]") {
    REQUIRE(fnv1a("") == 0xcbf29ce484222325ULL);
    REQUIRE(fnv1a("a") == 0xaf63dc4c8601ec8cULL);
    REQUIRE(fnv1a("foobar") == 0x85944171f73967e8ULL);
    REQUIRE(fnv1a("hash table") == 0xe197b6ec7d955fe5ULL);
}

TEST_CASE("Task 4: polyHash", "[task4]") {
    REQUIRE(polyHash("") == 0);
    REQUIRE(polyHash("a") == 97);
    REQUIRE(polyHash("foobar") == 26088312);
    REQUIRE(polyHash("hash table") == 392745816);
    REQUIRE(polyHash("abc", 256, 1'000'000'007) == 6382179);  // = 0x616263
    REQUIRE(polyHash("zzzzzzzzzzzz", 131, 4294967291ULL) == 572420930);  // no overflow
}

TEST_CASE("Task 4: anagrams collide under sumHash but not under good hashes", "[task4]") {
    const std::string words[] = {"listen", "silent", "enlist", "tinsel", "inlets"};
    std::set<std::uint64_t> sums, polys, fnvs;
    for (const auto& w : words) {
        sums.insert(sumHash(w));
        polys.insert(polyHash(w));
        fnvs.insert(fnv1a(w));
    }
    REQUIRE(sums.size() == 1);  // all five in ONE bucket
    REQUIRE(polys.size() == 5);
    REQUIRE(fnvs.size() == 5);
}
