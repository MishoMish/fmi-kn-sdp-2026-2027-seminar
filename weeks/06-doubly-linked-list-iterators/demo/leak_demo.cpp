// Live-coding demo for seminar 6: three ways to find a memory leak.
//
//   g++ -std=c++17 -g leak_demo.cpp -o demo && ./demo                       # counter says 2
//   g++ -std=c++17 -g -fsanitize=address leak_demo.cpp -o demo && ./demo    # LeakSanitizer report
//   valgrind --leak-check=full ./demo                                       # (Linux, without ASan)
#include <cstdio>

struct Node {
    static inline long alive = 0;
    int value;
    Node* prev;
    Node* next;
    Node(int v) : value(v), prev(nullptr), next(nullptr) { ++alive; }
    ~Node() { --alive; }
};

struct TinyList {
    Node sentinel{0};
    TinyList() { sentinel.prev = sentinel.next = &sentinel; }

    void pushBack(int v) {
        Node* n = new Node(v);
        n->prev = sentinel.prev;
        n->next = &sentinel;
        sentinel.prev->next = n;
        sentinel.prev = n;
    }

    // BUG: unlinks the node but never deletes it.
    void popFront() {
        Node* victim = sentinel.next;
        victim->prev->next = victim->next;
        victim->next->prev = victim->prev;
        // delete victim;
    }

    ~TinyList() {
        Node* n = sentinel.next;
        while (n != &sentinel) {
            Node* next = n->next;
            delete n;
            n = next;
        }
    }
};

int main() {
    {
        TinyList list;
        for (int i = 0; i < 5; ++i) list.pushBack(i);
        list.popFront();
        list.popFront();
    }
    std::printf("nodes still alive: %ld\n", Node::alive);
}
