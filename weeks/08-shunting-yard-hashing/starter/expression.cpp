#include "expression.h"

#include <cctype>
#include <stdexcept>

#include <stack>  // std::stack - the same adaptor you wrote in week 07

std::vector<Token> tokenize(const std::string& text) {
    std::vector<Token> out;
    for (std::size_t i = 0; i < text.size();) {
        const char c = text[i];
        if (std::isspace(static_cast<unsigned char>(c))) {
            ++i;
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            long long v = 0;
            while (i < text.size() && std::isdigit(static_cast<unsigned char>(text[i]))) {
                v = v * 10 + (text[i] - '0');
                ++i;
            }
            out.push_back(Token::number(v));
        } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
            out.push_back(Token::oper(c));
            ++i;
        } else if (c == '(') {
            out.push_back(Token::left());
            ++i;
        } else if (c == ')') {
            out.push_back(Token::right());
            ++i;
        } else {
            throw std::invalid_argument(std::string("tokenize: unexpected character '") + c + "'");
        }
    }
    return out;
}

std::string toString(const std::vector<Token>& tokens) {
    std::string s;
    for (const Token& t : tokens) {
        if (!s.empty()) s += ' ';
        if (t.kind == Token::Kind::Number) {
            s += std::to_string(t.value);
        } else {
            s += t.op;
        }
    }
    return s;
}

long long evalRPN(const std::vector<Token>& rpn) {
    // TODO: numbers -> push; operator -> pop right, pop left, push (left op right)
    (void)rpn;
    return 0;
}

std::vector<Token> toRPN(const std::vector<Token>& infix) {
    // TODO: see the notes, §2.3 - number -> output; operator -> pop while the
    // top binds tighter (or as tight and left-associative); ( -> push;
    // ) -> pop until (
    (void)infix;
    return {};
}

long long evaluate(const std::string& text) {
    // TODO: one line
    (void)text;
    return 0;
}
