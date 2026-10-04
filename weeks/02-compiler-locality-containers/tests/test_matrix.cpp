#include <catch_amalgamated.hpp>

#include <string>

#include "matrix.h"

TEST_CASE("Task 1: index() is row-major", "[task1]") {
    const Matrix<int> m(3, 4);
    REQUIRE(m.index(0, 0) == 0);
    REQUIRE(m.index(0, 3) == 3);
    REQUIRE(m.index(1, 0) == 4);
    REQUIRE(m.index(2, 1) == 9);
    REQUIRE(m.index(2, 3) == 11);
}

TEST_CASE("Task 1: every cell has its own slot", "[task1]") {
    Matrix<int> m(5, 7);
    for (std::size_t r = 0; r < m.rows(); ++r) {
        for (std::size_t c = 0; c < m.cols(); ++c) {
            m(r, c) = static_cast<int>(100 * r + c);
        }
    }
    for (std::size_t r = 0; r < m.rows(); ++r) {
        for (std::size_t c = 0; c < m.cols(); ++c) {
            INFO("r = " << r << ", c = " << c);
            REQUIRE(m(r, c) == static_cast<int>(100 * r + c));
            REQUIRE(m.data()[r * m.cols() + c] == m(r, c));
        }
    }
}

TEST_CASE("Task 1: works for non-square and degenerate shapes", "[task1]") {
    Matrix<std::string> row(1, 3, "x");
    row(0, 2) = "last";
    REQUIRE(row.data()[2] == "last");

    Matrix<double> column(4, 1);
    column(3, 0) = 2.5;
    REQUIRE(column.data()[3] == 2.5);
    REQUIRE(column.index(3, 0) == 3);
}
