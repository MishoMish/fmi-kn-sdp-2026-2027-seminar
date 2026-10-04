#include "floating.h"

#include <cmath>

bool isNaN(double x) {
    // TODO
    (void)x;
    return false;
}

bool isNegativeZero(double x) {
    // TODO: -0.0 == 0.0 is true, so == alone is not enough. What is 1.0 / -0.0?
    (void)x;
    return false;
}

bool almostEqual(double a, double b, double relTol, double absTol) {
    // TODO: this is exactly the comparison you should NOT use
    (void)relTol;
    (void)absTol;
    return a == b;
}

double kahanSum(const std::vector<double>& values) {
    // TODO
    (void)values;
    return 0.0;
}

bool nanLastLess(double a, double b) {
    // TODO. Returning false for everything is a valid (useless) ordering:
    // it says "all elements are equivalent", so std::sort will not crash.
    (void)a;
    (void)b;
    return false;
}
