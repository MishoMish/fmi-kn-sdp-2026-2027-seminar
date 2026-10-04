#ifndef SDP_W05_NODE_ALGORITHMS_H_
#define SDP_W05_NODE_ALGORITHMS_H_

#include <cstddef>

// Task 5 and 6: classic pointer algorithms on bare nodes.
// Here there is no list object guarding an invariant - a chain may even
// loop back on itself. None of these functions allocates or frees nodes
// (except josephus, which builds and destroys its own circle).

struct ListNode {
    int value = 0;
    ListNode* next = nullptr;
};

// Middle node: for length 5 the 3rd, for length 6 the 4th (the second of
// the two middles). nullptr for an empty chain. One pass, Theta(1) memory.
ListNode* middle(ListNode* head);

// k-th node from the end (k = 1 is the last). nullptr if k == 0 or k is
// larger than the length. One pass, Theta(1) memory.
ListNode* kthFromEnd(ListNode* head, std::size_t k);

// Does following next from head ever revisit a node? Theta(1) memory -
// no set of visited nodes (Floyd's tortoise and hare).
bool hasCycle(const ListNode* head);

// a and b are sorted ascending. Relink their nodes into one sorted chain
// and return its head. Stable: on equal values, a's node comes first.
// No node is allocated; every node of a and b ends up in the result.
ListNode* mergeSorted(ListNode* a, ListNode* b);

// Task 6: n people stand in a circle, numbered 1..n. Starting from person
// 1, count k people (person 1 is "1") and remove the k-th; continue
// counting from the next one. Return the number of the last survivor.
// Simulate it with a circular singly linked list of n nodes.
int josephus(int n, int k);

#endif  // SDP_W05_NODE_ALGORITHMS_H_
