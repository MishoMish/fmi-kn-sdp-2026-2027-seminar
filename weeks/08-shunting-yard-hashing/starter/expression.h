#ifndef SDP_W08_EXPRESSION_H_
#define SDP_W08_EXPRESSION_H_

#include <string>
#include <vector>

// Tasks 1-3: evaluating arithmetic expressions with stacks.
//
// Integers (long long), the binary operators + - * / ^ and parentheses.
// Precedence: ^ (highest), then * /, then + -.
// Associativity: + - * / are LEFT-associative (8 - 3 - 2 = (8 - 3) - 2),
//                ^ is RIGHT-associative       (2 ^ 3 ^ 2 = 2 ^ (3 ^ 2)).
// / is integer division (truncates toward zero, like C++). a ^ b needs b >= 0.

struct Token {
    enum class Kind { Number, Operator, LeftParen, RightParen };
    Kind kind;
    long long value = 0;  // for Number
    char op = 0;          // for Operator: one of + - * / ^

    static Token number(long long v) { return {Kind::Number, v, 0}; }
    static Token oper(char c) { return {Kind::Operator, 0, c}; }
    static Token left() { return {Kind::LeftParen, 0, '('}; }
    static Token right() { return {Kind::RightParen, 0, ')'}; }
    bool operator==(const Token& o) const { return kind == o.kind && value == o.value && op == o.op; }
};

// Done: "12 + 3*(4-1)" -> 12, +, 3, *, (, 4, -, 1, ).
// Spaces are ignored. std::invalid_argument on any other character.
std::vector<Token> tokenize(const std::string& text);

// Done: tokens -> "12 3 4 1 - * +" (for printing and for the tests).
std::string toString(const std::vector<Token>& tokens);

// Task 1 ★: evaluate a postfix (Reverse Polish) token sequence with a stack
// of numbers. std::invalid_argument if it is malformed (an operator without
// two operands, or more than one number left at the end, or a parenthesis).
// std::domain_error on division by zero or a negative exponent.
long long evalRPN(const std::vector<Token>& rpn);

// Task 2 ★★: Dijkstra's Shunting-yard - infix tokens to postfix tokens,
// using a stack of operators. std::invalid_argument on mismatched parentheses.
std::vector<Token> toRPN(const std::vector<Token>& infix);

// Task 3 ★: evaluate("12 + 3*(4-1)") == 21.
long long evaluate(const std::string& text);

#endif  // SDP_W08_EXPRESSION_H_
