#include <catch_amalgamated.hpp>

#include <algorithm>
#include <random>
#include <vector>

#include "node_algorithms.h"

namespace {

// Nodes live in a vector (no new/delete in the tests); link() chains them
// in order and returns the head.
ListNode* link(std::vector<ListNode>& nodes) {
    for (std::size_t i = 0; i + 1 < nodes.size(); ++i) {
        nodes[i].next = &nodes[i + 1];
    }
    if (!nodes.empty()) {
        nodes.back().next = nullptr;
    }
    return nodes.empty() ? nullptr : &nodes[0];
}

std::vector<ListNode> make(std::vector<int> values) {
    std::vector<ListNode> nodes(values.size());
    for (std::size_t i = 0; i < values.size(); ++i) {
        nodes[i].value = values[i];
    }
    return nodes;
}

std::vector<int> walk(const ListNode* head, std::size_t limit = 1000) {
    std::vector<int> out;
    for (; head != nullptr && out.size() < limit; head = head->next) {
        out.push_back(head->value);
    }
    return out;
}

}  // namespace

// ---------------------------------------------------------------- Task 5

TEST_CASE("Task 5: middle", "[task5]") {
    REQUIRE(middle(nullptr) == nullptr);
    for (int n = 1; n <= 12; ++n) {
        std::vector<int> values(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i) values[static_cast<std::size_t>(i)] = i;
        auto nodes = make(values);
        INFO("length " << n);
        REQUIRE(middle(link(nodes)) == &nodes[static_cast<std::size_t>(n / 2)]);
    }
}

TEST_CASE("Task 5: kthFromEnd", "[task5]") {
    auto nodes = make({10, 20, 30, 40});
    ListNode* head = link(nodes);
    REQUIRE(kthFromEnd(head, 1) == &nodes[3]);
    REQUIRE(kthFromEnd(head, 4) == &nodes[0]);
    REQUIRE(kthFromEnd(head, 2) == &nodes[2]);
    REQUIRE(kthFromEnd(head, 5) == nullptr);
    REQUIRE(kthFromEnd(head, 0) == nullptr);
    REQUIRE(kthFromEnd(nullptr, 1) == nullptr);
}

TEST_CASE("Task 5: hasCycle", "[task5]") {
    REQUIRE_FALSE(hasCycle(nullptr));
    auto nodes = make({1, 2, 3, 4, 5, 6, 7});
    ListNode* head = link(nodes);
    REQUIRE_FALSE(hasCycle(head));

    SECTION("tail back to the head") {
        nodes[6].next = &nodes[0];
        REQUIRE(hasCycle(head));
    }
    SECTION("tail back into the middle (rho shape)") {
        nodes[6].next = &nodes[3];
        REQUIRE(hasCycle(head));
    }
    SECTION("a node pointing to itself") {
        nodes[0].next = &nodes[0];
        REQUIRE(hasCycle(head));
    }
    SECTION("a long chain without a cycle") {
        std::vector<ListNode> many(100'000);
        REQUIRE_FALSE(hasCycle(link(many)));
        many.back().next = &many[50'000];
        REQUIRE(hasCycle(&many[0]));
    }
}

TEST_CASE("Task 5: mergeSorted relinks nodes into one sorted chain", "[task5]") {
    SECTION("simple") {
        auto a = make({1, 4, 6});
        auto b = make({2, 3, 7, 8});
        ListNode* m = mergeSorted(link(a), link(b));
        REQUIRE(walk(m) == std::vector<int>{1, 2, 3, 4, 6, 7, 8});
        REQUIRE(m == &a[0]);  // the very same nodes, not copies
    }
    SECTION("empty sides") {
        auto a = make({5, 9});
        REQUIRE(mergeSorted(link(a), nullptr) == &a[0]);
        REQUIRE(walk(mergeSorted(nullptr, &a[0])) == std::vector<int>{5, 9});
        REQUIRE(mergeSorted(nullptr, nullptr) == nullptr);
    }
    SECTION("stable: equal values keep a's node first") {
        auto a = make({1, 2, 2});
        auto b = make({2, 3});
        ListNode* m = mergeSorted(link(a), link(b));
        REQUIRE(walk(m) == std::vector<int>{1, 2, 2, 2, 3});
        REQUIRE(m->next == &a[1]);
        REQUIRE(m->next->next == &a[2]);
        REQUIRE(m->next->next->next == &b[0]);
    }
    SECTION("random") {
        std::mt19937 rng(2026);
        for (int round = 0; round < 200; ++round) {
            std::vector<int> va(rng() % 10), vb(rng() % 10);
            for (int& x : va) x = static_cast<int>(rng() % 20);
            for (int& x : vb) x = static_cast<int>(rng() % 20);
            std::sort(va.begin(), va.end());
            std::sort(vb.begin(), vb.end());
            std::vector<int> expected(va);
            expected.insert(expected.end(), vb.begin(), vb.end());
            std::sort(expected.begin(), expected.end());
            auto a = make(va);
            auto b = make(vb);
            INFO("round " << round);
            REQUIRE(walk(mergeSorted(link(a), link(b))) == expected);
        }
    }
}

// ---------------------------------------------------------------- Task 6

TEST_CASE("Task 6: josephus", "[task6]") {
    REQUIRE(josephus(1, 1) == 1);
    REQUIRE(josephus(1, 5) == 1);
    REQUIRE(josephus(5, 1) == 5);   // everyone in order; the last stays
    REQUIRE(josephus(5, 2) == 3);   // 2, 4, 1, 5 leave
    REQUIRE(josephus(7, 3) == 4);
    REQUIRE(josephus(41, 3) == 31);  // the historical version
    // Compare with the recurrence J(1) = 0, J(n) = (J(n-1) + k) mod n.
    for (int k = 1; k <= 6; ++k) {
        int j = 0;
        for (int n = 1; n <= 60; ++n) {
            if (n > 1) j = (j + k) % n;
            INFO("n = " << n << ", k = " << k);
            REQUIRE(josephus(n, k) == j + 1);
        }
    }
}
