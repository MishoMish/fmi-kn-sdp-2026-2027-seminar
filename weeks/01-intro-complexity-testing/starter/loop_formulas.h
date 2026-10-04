#ifndef SDP_W01_LOOP_FORMULAS_H_
#define SDP_W01_LOOP_FORMULAS_H_

#include <cstdint>

// Task 1: closed-form iteration counts.
//
// tests/loops.h contains six small loops. Each one counts how many times
// its innermost statement runs. For every loop, derive an exact formula
// in terms of n and implement it below WITHOUT any loop that simulates
// the original (a loop is allowed only in formulaHalvingSum, for counting bits).
//
// The tests compare your formula with the real count for many values of n.

std::uint64_t formulaStepped(std::uint64_t n);     // easy
std::uint64_t formulaSquare(std::uint64_t n);      // easy
std::uint64_t formulaTriangle(std::uint64_t n);    // easy
std::uint64_t formulaDoubling(std::uint64_t n);    // medium
std::uint64_t formulaTriple(std::uint64_t n);      // medium
std::uint64_t formulaHalvingSum(std::uint64_t n);  // hard

#endif  // SDP_W01_LOOP_FORMULAS_H_
