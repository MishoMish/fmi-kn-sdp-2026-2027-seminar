#ifndef SDP_W05_SINGLY_LINKED_LIST_H_
#define SDP_W05_SINGLY_LINKED_LIST_H_

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>

// A singly linked list with a pointer to the last node, so that both
// pushFront and pushBack are Theta(1):
//
//   head_ -> [a|*] -> [b|*] -> [c|nullptr]
//                               ^
//   tail_ ----------------------'          size_ = 3
//
// Invariant:
//   - size_ == number of nodes reachable from head_;
//   - size_ == 0  <=>  head_ == nullptr  <=>  tail_ == nullptr;
//   - otherwise tail_ is the last node and tail_->next == nullptr;
//   - every node is owned by the list (allocated with new, freed with delete).
//
// Positions are indices 0 .. size()-1; reaching index i costs Theta(i).
// (Next week: iterators instead of indices.)
template <typename T>
class SinglyLinkedList {
public:
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    SinglyLinkedList() noexcept = default;

    SinglyLinkedList(std::initializer_list<T> values) {
        for (const T& v : values) {
            pushBack(v);
        }
    }

    ~SinglyLinkedList() { clear(); }

    // ---- Task 3: the rule of five --------------------------------------

    // Deep copy in the same order. Must be Theta(n) - not Theta(n^2).
    SinglyLinkedList(const SinglyLinkedList& other) {
        // TODO
        (void)other;
    }

    // Steal other's nodes; other becomes empty. No node is allocated.
    SinglyLinkedList(SinglyLinkedList&& other) noexcept {
        // TODO
        (void)other;
    }

    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        // TODO (copy-and-swap)
        (void)other;
        return *this;
    }

    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        // TODO
        (void)other;
        return *this;
    }

    // ---- queries (done) -------------------------------------------------

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    T& front() {
        requireNonEmpty("front");
        return head_->value;
    }
    const T& front() const {
        requireNonEmpty("front");
        return head_->value;
    }
    T& back() {
        requireNonEmpty("back");
        return tail_->value;
    }
    const T& back() const {
        requireNonEmpty("back");
        return tail_->value;
    }

    // Theta(i). Throws std::out_of_range if i >= size().
    T& at(std::size_t i) { return nodeAt(i)->value; }
    const T& at(std::size_t i) const { return nodeAt(i)->value; }

    // Calls f(value) for every element, front to back.
    template <typename F>
    void forEach(F&& f) const {
        for (const Node* n = head_; n != nullptr; n = n->next) {
            f(n->value);
        }
    }

    void swap(SinglyLinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

    bool operator==(const SinglyLinkedList& other) const {
        if (size_ != other.size_) return false;
        for (const Node *a = head_, *b = other.head_; a != nullptr; a = a->next, b = b->next) {
            if (!(a->value == b->value)) return false;
        }
        return true;
    }
    bool operator!=(const SinglyLinkedList& other) const { return !(*this == other); }

    // ---- Task 1: the ends --------------------------------------------------

    void pushFront(const T& value) {
        // TODO: new node in front of head_; mind the empty list (tail_!)
        (void)value;
    }

    void pushBack(const T& value) {
        // TODO: new node after tail_; mind the empty list (head_!)
        (void)value;
    }

    // Throw std::out_of_range if empty.
    void popFront() {
        // TODO: mind the list that becomes empty (tail_!)
    }

    // Delete every node. Iteratively - a recursive delete would overflow
    // the call stack on a list with a million elements.
    void clear() noexcept {
        // TODO
    }

    // ---- Task 2: positions -------------------------------------------------

    // value ends up at index i; i == size() appends. std::out_of_range if i > size().
    void insertAt(std::size_t i, const T& value) {
        // TODO
        (void)i;
        (void)value;
    }

    // std::out_of_range if i >= size().
    void removeAt(std::size_t i) {
        // TODO: careful when removing the last node (tail_!)
        (void)i;
    }

    // Index of the first element equal to value, or npos.
    std::size_t indexOf(const T& value) const {
        // TODO
        (void)value;
        return npos;
    }

    // ---- Task 4: whole-list algorithms ---------------------------------

    // Reverse the order of the nodes in place: Theta(n) time, Theta(1)
    // extra memory, no node is allocated or freed, no value is copied.
    void reverse() noexcept {
        // TODO
    }

    // Remove every element equal to value; return how many were removed.
    std::size_t removeAll(const T& value) {
        // TODO
        (void)value;
        return 0;
    }

private:
    struct Node {
        T value;
        Node* next = nullptr;
    };

    void requireNonEmpty(const char* what) const {
        if (empty()) {
            throw std::out_of_range(std::string("SinglyLinkedList::") + what + ": empty list");
        }
    }

    Node* nodeAt(std::size_t i) const {
        if (i >= size_) {
            throw std::out_of_range("SinglyLinkedList: index out of range");
        }
        Node* n = head_;
        while (i-- > 0) {
            n = n->next;
        }
        return n;
    }

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

#endif  // SDP_W05_SINGLY_LINKED_LIST_H_
