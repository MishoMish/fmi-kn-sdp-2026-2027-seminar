#include <catch_amalgamated.hpp>

#include <cstdint>
#include <random>

#include "matrix.h"
#include "traversal.h"

namespace {

Matrix<std::int64_t> numbered(std::size_t rows, std::size_t cols) {
    Matrix<std::int64_t> m(rows, cols);
    std::int64_t next = 1;
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            m(r, c) = next++;
        }
    }
    return m;
}

void checkTranspose(const Matrix<std::int64_t>& m, const Matrix<std::int64_t>& t) {
    REQUIRE(t.rows() == m.cols());
    REQUIRE(t.cols() == m.rows());
    for (std::size_t r = 0; r < m.rows(); ++r) {
        for (std::size_t c = 0; c < m.cols(); ++c) {
            INFO("r = " << r << ", c = " << c);
            REQUIRE(t(c, r) == m(r, c));
        }
    }
}

}  // namespace

TEST_CASE("Task 2: both traversal orders give the same sum", "[task2]") {
    const auto m = numbered(37, 53);  // 1 + 2 + ... + 1961
    const std::int64_t expected = 1961LL * 1962 / 2;
    REQUIRE(sumRowMajor(m) == expected);
    REQUIRE(sumColMajor(m) == expected);
}

TEST_CASE("Task 2: sums of small and empty matrices", "[task2]") {
    REQUIRE(sumRowMajor(Matrix<int>(0, 0)) == 0);
    REQUIRE(sumColMajor(Matrix<int>(0, 5)) == 0);
    REQUIRE(sumRowMajor(Matrix<int>(1, 1, 7)) == 7);
    REQUIRE(sumColMajor(Matrix<int>(3, 2, -1)) == -6);
}

TEST_CASE("Task 2: transpose", "[task2]") {
    SECTION("square") { checkTranspose(numbered(4, 4), transpose(numbered(4, 4))); }
    SECTION("wide") { checkTranspose(numbered(2, 7), transpose(numbered(2, 7))); }
    SECTION("tall") { checkTranspose(numbered(9, 3), transpose(numbered(9, 3))); }
    SECTION("transposing twice gives the original") {
        const auto m = numbered(5, 8);
        REQUIRE(transpose(transpose(m)) == m);
    }
}

TEST_CASE("Task 4: blocked transpose matches the naive one", "[task4]") {
    std::mt19937 rng(2026);
    for (int round = 0; round < 40; ++round) {
        const std::size_t rows = 1 + rng() % 70;
        const std::size_t cols = 1 + rng() % 70;
        const std::size_t block = 1 + rng() % 20;
        INFO(rows << " x " << cols << ", block " << block);
        const auto m = numbered(rows, cols);
        checkTranspose(m, transposeBlocked(m, block));
    }
    SECTION("default block size on a larger matrix") {
        const auto m = numbered(130, 97);
        checkTranspose(m, transposeBlocked(m));
    }
}
