#ifndef SDP_W02_MATRIX_H_
#define SDP_W02_MATRIX_H_

#include <cstddef>
#include <vector>

// Task 1: a rows x cols matrix stored in ONE contiguous block, row by row
// ("row-major" order, like C/C++ 2D arrays):
//
//   (0,0) (0,1) ... (0,cols-1) (1,0) (1,1) ... (rows-1,cols-1)
//
// Only index() is missing. Everything else is built on top of it.
template <typename T>
class Matrix {
public:
    Matrix(std::size_t rows, std::size_t cols, const T& init = T{})
        : rows_(rows), cols_(cols), cells_(rows * cols, init) {}

    std::size_t rows() const noexcept { return rows_; }
    std::size_t cols() const noexcept { return cols_; }
    std::size_t size() const noexcept { return cells_.size(); }

    // Position of element (row, col) inside the contiguous block.
    std::size_t index(std::size_t row, std::size_t col) const noexcept {
        // TODO: row-major formula
        (void)row;
        (void)col;
        return 0;
    }

    T& operator()(std::size_t row, std::size_t col) { return cells_[index(row, col)]; }
    const T& operator()(std::size_t row, std::size_t col) const { return cells_[index(row, col)]; }

    T* data() noexcept { return cells_.data(); }
    const T* data() const noexcept { return cells_.data(); }

    bool operator==(const Matrix& other) const {
        return rows_ == other.rows_ && cols_ == other.cols_ && cells_ == other.cells_;
    }
    bool operator!=(const Matrix& other) const { return !(*this == other); }

private:
    std::size_t rows_;
    std::size_t cols_;
    std::vector<T> cells_;
};

#endif  // SDP_W02_MATRIX_H_
