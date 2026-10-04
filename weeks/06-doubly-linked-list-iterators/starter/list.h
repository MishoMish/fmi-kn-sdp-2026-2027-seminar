#ifndef SDP_W06_LIST_H_
#define SDP_W06_LIST_H_

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// A doubly linked list with a SENTINEL node, like std::list.
//
// The sentinel is a node without a value that sits "before the first and
// after the last" element; the list is a circle through it:
//
//         +--------------------------------------------+
//         v                                            |
//   [sentinel] <-> [a] <-> [b] <-> [c] <---------------+
//         ^ end()   ^ begin()
//
// An empty list is the sentinel pointing to itself. Because every real
// node always has a real prev and a real next, insert and erase need NO
// special cases for "first", "last" or "empty" - compare with week 05.
//
// Invariant: following next from the sentinel visits exactly size_ nodes
// and returns to the sentinel; for every node n, n->next->prev == n.
template <typename T>
class List {
    struct NodeBase {
        NodeBase* prev;
        NodeBase* next;
    };
    struct Node : NodeBase {
        T value;
        Node(const T& v) : NodeBase{nullptr, nullptr}, value(v) {}  // NOLINT
    };

public:
    // ---- Task 1: the iterator -------------------------------------------
    //
    // One template for both iterator (IsConst = false) and const_iterator
    // (IsConst = true). It holds a pointer to a node; end() holds the
    // sentinel. The member types below make it a real STL bidirectional
    // iterator, so std::find, std::count, std::reverse, std::distance, ...
    // all work with it.
    template <bool IsConst>
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        Iterator() = default;

        // iterator -> const_iterator is allowed (not the other way round).
        template <bool C = IsConst, typename = std::enable_if_t<C>>
        Iterator(const Iterator<false>& other) : node_(other.node_) {}  // NOLINT

        // The value of the node. (Undefined for end(), as in the STL.)
        reference operator*() const {
            // TODO: the node is really a Node - static_cast and return its value
            throw std::logic_error("TODO: Iterator::operator*");
        }

        pointer operator->() const {
            // TODO: address of the value
            throw std::logic_error("TODO: Iterator::operator->");
        }

        Iterator& operator++() {  // ++it: move to next, return *this
            // TODO
            return *this;
        }

        Iterator operator++(int) {  // it++: move to next, return the OLD position
            // TODO
            return *this;
        }

        Iterator& operator--() {  // --end() is the last element
            // TODO
            return *this;
        }

        Iterator operator--(int) {
            // TODO
            return *this;
        }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            // TODO: the same node
            (void)a;
            (void)b;
            return true;
        }
        friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); }

    private:
        friend class List;
        template <bool>
        friend class Iterator;
        explicit Iterator(NodeBase* node) : node_(node) {}

        NodeBase* node_ = nullptr;
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using value_type = T;
    using size_type = std::size_t;

    // ---- construction ---------------------------------------------------

    List() noexcept = default;

    List(std::initializer_list<T> values) : List() {
        for (const T& v : values) {
            pushBack(v);
        }
    }

    ~List() { clear(); }

    // ---- Task 3: the rule of five ----------------------------------------

    // Copy every element in order. If a copy throws, the destructor of this
    // half-built object will NOT run - free what was built, then rethrow.
    List(const List& other) : List() {
        // TODO
        (void)other;
    }

    // Done - both are built on stealNodes (below).
    List(List&& other) noexcept : List() { stealNodes(other); }
    List& operator=(List&& other) noexcept {
        if (this != &other) {
            clear();
            stealNodes(other);
        }
        return *this;
    }

    List& operator=(const List& other) {
        // TODO (copy-and-swap; swap is done)
        (void)other;
        return *this;
    }

    // ---- iterators and queries (done) -------------------------------------

    iterator begin() noexcept { return iterator(sentinel_.next); }
    iterator end() noexcept { return iterator(&sentinel_); }
    const_iterator begin() const noexcept { return const_iterator(sentinel_.next); }
    const_iterator end() const noexcept { return const_iterator(const_cast<NodeBase*>(&sentinel_)); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    T& front() { return *begin(); }
    T& back() { return *std::prev(end()); }
    const T& front() const { return *begin(); }
    const T& back() const { return *std::prev(end()); }

    // ---- Task 2: insert and erase ------------------------------------------

    // Insert value BEFORE pos (pos may be end()); return an iterator to it.
    // Four pointers change. No special cases - thanks to the sentinel.
    iterator insert(const_iterator pos, const T& value) {
        // TODO
        (void)pos;
        (void)value;
        throw std::logic_error("TODO: List::insert");
    }

    // Erase the element at pos (pos != end()); return the iterator after it.
    iterator erase(const_iterator pos) {
        // TODO
        (void)pos;
        throw std::logic_error("TODO: List::erase");
    }

    // Erase every element. Iteratively. Afterwards the list is empty and usable.
    void clear() noexcept {
        // TODO
    }

    // Everything below is built on insert and erase (done).
    void pushFront(const T& v) { insert(begin(), v); }
    void pushBack(const T& v) { insert(end(), v); }
    void popFront() {
        requireNonEmpty("popFront");
        erase(begin());
    }
    void popBack() {
        requireNonEmpty("popBack");
        erase(std::prev(end()));
    }

    // ---- Task 4: whole-list operations in Theta(1) / Theta(n) ------------

    // Move ALL nodes of other into this list, before pos. Theta(1): no node
    // is allocated, copied or freed - only pointers change. other becomes empty.
    void splice(const_iterator pos, List& other) noexcept {
        // TODO
        (void)pos;
        (void)other;
    }

    // Reverse in place: in every node (and the sentinel) swap prev and next.
    void reverse() noexcept {
        // TODO
    }

    // Done - three steals through an empty temporary.
    void swap(List& other) noexcept {
        List tmp;
        tmp.stealNodes(*this);
        stealNodes(other);
        other.stealNodes(tmp);
    }

    bool operator==(const List& other) const {
        if (size_ != other.size_) return false;
        for (auto a = begin(), b = other.begin(); a != end(); ++a, ++b) {
            if (!(*a == *b)) return false;
        }
        return true;
    }
    bool operator!=(const List& other) const { return !(*this == other); }

private:
    // Task 3: precondition - *this is empty. Take all of other's nodes.
    // Careful: the sentinel is a MEMBER, so it does not move with the nodes.
    // other's first and last node point to other's sentinel - relink them to
    // ours. Afterwards other is empty (its sentinel points to itself).
    void stealNodes(List& other) noexcept {
        // TODO
        (void)other;
    }

    void requireNonEmpty(const char* what) const {
        if (empty()) {
            throw std::out_of_range(std::string("List::") + what + ": empty list");
        }
    }

    NodeBase sentinel_{&sentinel_, &sentinel_};
    std::size_t size_ = 0;
};

#endif  // SDP_W06_LIST_H_
