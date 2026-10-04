#include <catch_amalgamated.hpp>

#include <cctype>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "expression.h"

namespace {

std::vector<Token> rpn(const std::string& text) { return toRPN(tokenize(text)); }

// An independent reference: recursive descent over + - * / with
// parentheses (no ^), used to check evaluate() on random expressions.
struct Reference {
    const std::string& s;
    std::size_t i = 0;
    long long expr() {
        long long v = term();
        while (i < s.size() && (s[i] == '+' || s[i] == '-')) {
            const char op = s[i++];
            const long long r = term();
            v = op == '+' ? v + r : v - r;
        }
        return v;
    }
    long long term() {
        long long v = factor();
        while (i < s.size() && (s[i] == '*' || s[i] == '/')) {
            const char op = s[i++];
            const long long r = factor();
            if (op == '/' && r == 0) throw std::domain_error("reference: division by zero");
            v = op == '*' ? v * r : v / r;
        }
        return v;
    }
    long long factor() {
        if (s[i] == '(') {
            ++i;
            const long long v = expr();
            ++i;  // ')'
            return v;
        }
        long long v = 0;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) v = v * 10 + (s[i++] - '0');
        return v;
    }
};

std::string randomExpr(std::mt19937& rng, int depth) {
    if (depth == 0 || rng() % 3 == 0) return std::to_string(1 + rng() % 9);
    static const char ops[] = "+-*/";
    std::string l = randomExpr(rng, depth - 1);
    std::string r = randomExpr(rng, depth - 1);
    std::string e = l + ops[rng() % 4] + r;
    return rng() % 2 ? "(" + e + ")" : e;
}

}  // namespace

// ---------------------------------------------------------------- Task 1

TEST_CASE("Task 1: evalRPN", "[task1]") {
    using T = Token;
    REQUIRE(evalRPN({T::number(42)}) == 42);
    REQUIRE(evalRPN({T::number(3), T::number(4), T::oper('+')}) == 7);
    // 12 3 4 1 - * +  ==  12 + 3 * (4 - 1)
    REQUIRE(evalRPN({T::number(12), T::number(3), T::number(4), T::number(1), T::oper('-'), T::oper('*'),
                     T::oper('+')}) == 21);
    REQUIRE(evalRPN({T::number(8), T::number(3), T::oper('-')}) == 5);   // order matters: 8 - 3
    REQUIRE(evalRPN({T::number(7), T::number(2), T::oper('/')}) == 3);   // integer division
    REQUIRE(evalRPN({T::number(2), T::number(10), T::oper('^')}) == 1024);
    REQUIRE(evalRPN({T::number(5), T::number(0), T::oper('^')}) == 1);
}

TEST_CASE("Task 1: evalRPN rejects malformed input", "[task1]") {
    using T = Token;
    REQUIRE_THROWS_AS(evalRPN({}), std::invalid_argument);
    REQUIRE_THROWS_AS(evalRPN({T::oper('+')}), std::invalid_argument);
    REQUIRE_THROWS_AS(evalRPN({T::number(1), T::oper('+')}), std::invalid_argument);
    REQUIRE_THROWS_AS(evalRPN({T::number(1), T::number(2)}), std::invalid_argument);  // two left over
    REQUIRE_THROWS_AS(evalRPN({T::number(1), T::number(0), T::oper('/')}), std::domain_error);
    REQUIRE_THROWS_AS(evalRPN({T::number(2), T::number(0), T::number(1), T::oper('-'), T::oper('^')}),
                      std::domain_error);
}

// ---------------------------------------------------------------- Task 2

TEST_CASE("Task 2: toRPN - precedence", "[task2]") {
    REQUIRE(toString(rpn("1 + 2")) == "1 2 +");
    REQUIRE(toString(rpn("1 + 2 * 3")) == "1 2 3 * +");
    REQUIRE(toString(rpn("1 * 2 + 3")) == "1 2 * 3 +");
    REQUIRE(toString(rpn("12 + 3*(4-1)")) == "12 3 4 1 - * +");
    REQUIRE(toString(rpn("2 * 3 ^ 2")) == "2 3 2 ^ *");
}

TEST_CASE("Task 2: toRPN - associativity", "[task2]") {
    REQUIRE(toString(rpn("8 - 3 - 2")) == "8 3 - 2 -");      // left: (8-3)-2
    REQUIRE(toString(rpn("100 / 10 / 5")) == "100 10 / 5 /");
    REQUIRE(toString(rpn("2 ^ 3 ^ 2")) == "2 3 2 ^ ^");      // right: 2^(3^2)
}

TEST_CASE("Task 2: toRPN - parentheses", "[task2]") {
    REQUIRE(toString(rpn("(1 + 2) * 3")) == "1 2 + 3 *");
    REQUIRE(toString(rpn("((7))")) == "7");
    REQUIRE(toString(rpn("(1 - (2 - (3 - 4)))")) == "1 2 3 4 - - -");
    REQUIRE_THROWS_AS(rpn("(1 + 2"), std::invalid_argument);
    REQUIRE_THROWS_AS(rpn("1 + 2)"), std::invalid_argument);
    REQUIRE_THROWS_AS(rpn(")("), std::invalid_argument);
}

// ---------------------------------------------------------------- Task 3

TEST_CASE("Task 3: evaluate", "[task3]") {
    REQUIRE(evaluate("12 + 3*(4-1)") == 21);
    REQUIRE(evaluate("8 - 3 - 2") == 3);
    REQUIRE(evaluate("2 ^ 3 ^ 2") == 512);
    REQUIRE(evaluate("(2 ^ 3) ^ 2") == 64);
    REQUIRE(evaluate("3 + 4 * (2 - 1)") == 7);
    REQUIRE(evaluate("100 / 10 / 5") == 2);
    REQUIRE_THROWS_AS(evaluate("1 / (2 - 2)"), std::domain_error);
    REQUIRE_THROWS_AS(evaluate("1 +"), std::invalid_argument);
    REQUIRE_THROWS_AS(evaluate("1 + x"), std::invalid_argument);
}

TEST_CASE("Task 3: evaluate agrees with an independent parser", "[task3]") {
    std::mt19937 rng(2026);
    int checked = 0;
    while (checked < 500) {
        const std::string e = randomExpr(rng, 4);
        long long expected = 0;
        try {
            Reference ref{e};
            expected = ref.expr();
        } catch (const std::domain_error&) {
            continue;  // division by zero somewhere: skip this expression
        }
        INFO(e);
        REQUIRE(evaluate(e) == expected);
        ++checked;
    }
}
