#ifndef SDP_W01_DUPLICATES_H_
#define SDP_W01_DUPLICATES_H_

#include <algorithm>
#include <unordered_set>
#include <vector>

// Task 2: one problem, three algorithms.
// Each function returns true if some value occurs at least twice.
// All three must give the same answer; they differ only in cost.
//
// These are templates, so the whole implementation lives in the header.

// Compare every pair of elements.
// Expected cost: Theta(n^2) time, Theta(1) extra memory.
template <typename T>
bool hasDuplicateNaive(const std::vector<T>& values) {
    // TODO
    (void)values;
    return false;
}

// Sort a copy, then equal values end up next to each other.
// Expected cost: Theta(n log n) time, Theta(n) extra memory (the copy).
// Note: the parameter is taken BY VALUE on purpose - that is the copy.
template <typename T>
bool hasDuplicateSorting(std::vector<T> values) {
    // TODO: std::sort, then look at neighbours
    (void)values;
    return false;
}

// Remember what you have seen in a hash set.
// Expected cost: Theta(n) time on average, Theta(n) extra memory.
template <typename T>
bool hasDuplicateHashing(const std::vector<T>& values) {
    // TODO: std::unordered_set<T>; insert() tells you if the value was new
    (void)values;
    return false;
}

#endif  // SDP_W01_DUPLICATES_H_
