#include <catch_amalgamated.hpp>

#include <forward_list>
#include <iterator>
#include <string>
#include <vector>

#include "iter_algorithms.h"
#include "list.h"

namespace {

// A random-access iterator over an int array that counts how many times
// ++ was called - to check that distanceBetween does NOT walk.
struct CountingIt {
    using iterator_category = std::random_access_iterator_tag;
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = int*;
    using reference = int&;
    static inline long increments = 0;

    int* p = nullptr;
    reference operator*() const { return *p; }
    CountingIt& operator++() {
        ++increments;
        ++p;
        return *this;
    }
    CountingIt operator++(int) {
        CountingIt old = *this;
        ++*this;
        return old;
    }
    difference_type operator-(const CountingIt& o) const { return p - o.p; }
    bool operator==(const CountingIt& o) const { return p == o.p; }
    bool operator!=(const CountingIt& o) const { return p != o.p; }
};

}  // namespace

TEST_CASE("Task 5: findValue on four kinds of containers", "[task5]") {
    const std::vector<int> v{4, 8, 15, 16};
    REQUIRE(findValue(v.begin(), v.end(), 15) == v.begin() + 2);
    REQUIRE(findValue(v.begin(), v.end(), 99) == v.end());
    const int a[] = {1, 2, 3};
    REQUIRE(findValue(std::begin(a), std::end(a), 3) == a + 2);
    const std::forward_list<std::string> f{"x", "y"};
    REQUIRE(*findValue(f.begin(), f.end(), std::string("y")) == "y");
    List<int> list{7, 8};
    REQUIRE(findValue(list.begin(), list.end(), 8) == std::next(list.begin()));
    REQUIRE(findValue(list.begin(), list.end(), 9) == list.end());
}

TEST_CASE("Task 5: reverseRange", "[task5]") {
    for (int n = 0; n <= 7; ++n) {
        std::vector<int> v(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i) v[static_cast<std::size_t>(i)] = i;
        std::vector<int> expected(v.rbegin(), v.rend());
        INFO("length " << n);
        reverseRange(v.begin(), v.end());
        REQUIRE(v == expected);
    }
    List<std::string> list{"a", "b", "c", "d"};
    reverseRange(list.begin(), list.end());
    REQUIRE(std::vector<std::string>(list.begin(), list.end()) == std::vector<std::string>{"d", "c", "b", "a"});
}

TEST_CASE("Task 5: isPalindrome", "[task5]") {
    const std::string yes = "racecar";
    const std::string no = "racecars";
    REQUIRE(isPalindrome(yes.begin(), yes.end()));
    REQUIRE_FALSE(isPalindrome(no.begin(), no.end()));
    const std::string empty;
    REQUIRE(isPalindrome(empty.begin(), empty.end()));
    List<int> l1{1, 2, 2, 1};
    List<int> l2{1, 2, 3};
    REQUIRE(isPalindrome(l1.begin(), l1.end()));
    REQUIRE_FALSE(isPalindrome(l2.begin(), l2.end()));
}

TEST_CASE("Task 5: distanceBetween picks the right algorithm", "[task5]") {
    List<int> list{1, 2, 3, 4, 5};
    REQUIRE(distanceBetween(list.begin(), list.end()) == 5);
    const std::forward_list<int> f{1, 2, 3};
    REQUIRE(distanceBetween(f.begin(), f.end()) == 3);

    int data[1000] = {};
    CountingIt::increments = 0;
    REQUIRE(distanceBetween(CountingIt{data}, CountingIt{data + 1000}) == 1000);
    REQUIRE(CountingIt::increments == 0);  // Theta(1): no walking
}
