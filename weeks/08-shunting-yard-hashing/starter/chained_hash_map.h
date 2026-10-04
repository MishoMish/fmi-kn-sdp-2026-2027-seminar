#ifndef SDP_W08_CHAINED_HASH_MAP_H_
#define SDP_W08_CHAINED_HASH_MAP_H_

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

// Task 5 ★★: a hash map with SEPARATE CHAINING.
//
// buckets_ is a vector of buckets; each bucket is a small vector of
// (key, value) pairs whose keys hash to that bucket:
//
//   bucket index = hash(key) % buckets_.size()
//
// When size() / bucketCount() would exceed maxLoadFactor() (1.0), the
// table REHASHES: twice as many buckets, every pair re-inserted into its
// new bucket. With a decent hash, every operation is Theta(1) on average.
//
// Hash is a function object, like std::unordered_map's: std::hash<K> by
// default, or anything callable as hash(key) -> std::size_t (the tests
// also pass a deliberately terrible one).
template <typename K, typename V, typename Hash = std::hash<K>>
class ChainedHashMap {
public:
    explicit ChainedHashMap(std::size_t buckets = 8, Hash hash = Hash())
        : buckets_(buckets == 0 ? 1 : buckets), hash_(hash) {}

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    std::size_t bucketCount() const noexcept { return buckets_.size(); }
    double loadFactor() const noexcept { return static_cast<double>(size_) / buckets_.size(); }
    static constexpr double maxLoadFactor() { return 1.0; }

    // Insert (key, value) if key is absent; return true if inserted, false
    // if the key was already there (then the value is NOT changed).
    bool insert(const K& key, const V& value) {
        // TODO: look in the bucket; if absent, maybe rehash, then append
        (void)key;
        (void)value;
        return false;
    }

    // Pointer to the value for key, or nullptr.
    V* find(const K& key) {
        // TODO
        (void)key;
        return nullptr;
    }
    const V* find(const K& key) const {
        // TODO (same as above)
        (void)key;
        return nullptr;
    }

    bool contains(const K& key) const { return find(key) != nullptr; }

    // The value for key; if absent, insert V{} first (like std::unordered_map).
    V& operator[](const K& key) {
        // TODO
        (void)key;
        throw std::logic_error("TODO: ChainedHashMap::operator[]");
    }

    // Remove key; return true if it was there.
    bool erase(const K& key) {
        // TODO (order inside a bucket does not matter: swap with the last, pop_back)
        (void)key;
        return false;
    }

    // Length of the longest chain - how unlucky the hash is.
    std::size_t longestChain() const {
        std::size_t best = 0;
        for (const auto& b : buckets_) best = b.size() > best ? b.size() : best;
        return best;
    }

    // Calls f(key, value) for every pair, in no particular order.
    template <typename F>
    void forEach(F&& f) const {
        for (const auto& b : buckets_)
            for (const auto& kv : b) f(kv.first, kv.second);
    }

private:
    using Bucket = std::vector<std::pair<K, V>>;

    std::size_t indexFor(const K& key) const { return hash_(key) % buckets_.size(); }

    // TODO: new table with newBuckets buckets; move every pair into the
    // bucket it belongs to NOW (the index depends on the bucket count!)
    void rehash(std::size_t newBuckets) {
        (void)newBuckets;
    }

    std::vector<Bucket> buckets_;
    std::size_t size_ = 0;
    Hash hash_;
};

#endif  // SDP_W08_CHAINED_HASH_MAP_H_
