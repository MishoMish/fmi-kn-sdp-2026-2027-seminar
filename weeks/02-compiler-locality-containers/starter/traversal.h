#ifndef SDP_W02_TRAVERSAL_H_
#define SDP_W02_TRAVERSAL_H_

#include <algorithm>
#include <cstddef>

#include "matrix.h"

// Task 2: the same work in two different orders.
// Both functions must return the same sum. Run w02_bench to see what the
// order costs.

// Outer loop over rows, inner over columns: walks memory sequentially.
template <typename T>
T sumRowMajor(const Matrix<T>& m) {
    T total{};
    // TODO
    (void)m;
    return total;
}

// Outer loop over columns, inner over rows: jumps cols() elements each step.
template <typename T>
T sumColMajor(const Matrix<T>& m) {
    T total{};
    // TODO
    (void)m;
    return total;
}

// Task 2: result(c, r) == m(r, c). The result has m.cols() rows.
template <typename T>
Matrix<T> transpose(const Matrix<T>& m) {
    Matrix<T> result(m.cols(), m.rows());
    // TODO
    return result;
}

// Task 4: the same result, but processed in block x block tiles so that
// both the source tile and the destination tile stay in the cache.
// Must work for any sizes, including ones that are not multiples of block.
template <typename T>
Matrix<T> transposeBlocked(const Matrix<T>& m, std::size_t block = 32) {
    Matrix<T> result(m.cols(), m.rows());
    // TODO
    (void)block;
    return result;
}

#endif  // SDP_W02_TRAVERSAL_H_
