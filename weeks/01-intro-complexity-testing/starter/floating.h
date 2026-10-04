#ifndef SDP_W01_FLOATING_H_
#define SDP_W01_FLOATING_H_

#include <vector>

// Task 5: living with IEEE 754 doubles.

// True if x is NaN. Do NOT use std::isnan - use a property of NaN itself.
bool isNaN(double x);

// True only for -0.0 (not for +0.0, not for anything else).
// Do NOT use std::signbit.
bool isNegativeZero(double x);

// True if a and b are "equal enough":
//   |a - b| <= max(relTol * max(|a|, |b|), absTol)
// Equal infinities are equal; NaN is never equal to anything.
bool almostEqual(double a, double b, double relTol = 1e-9, double absTol = 1e-12);

// Task 5 (hard): Kahan compensated summation. Much smaller rounding error
// than adding the values one by one.
double kahanSum(const std::vector<double>& values);

// Task 6 (hard): a comparator usable with std::sort that orders numbers
// ascending and puts every NaN at the end. It must be a strict weak ordering.
bool nanLastLess(double a, double b);

#endif  // SDP_W01_FLOATING_H_
