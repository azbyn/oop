#pragma once
#include <string>

struct Token {
    enum Type {
        Eof,        // only to be used inside the parser
        Int,        // duh
        Var,        // identifier like sqrt...
        LeftParen,  // (
        RightParen, // (
        Lambda,     // λ or '\'
        Dot,        // . (for λ a. a)
        Plus,       // +
        Minus,      // -
        Div,        // /
        Mul,        // *
        Mod,        // %
        Pow,        // ^
        Eq,         // ==
        Ne,         // !=
        Lt,         // <=
        Gt,         // >=
        Le,         // <
        Ge,         // >
        And,        // &&
        Or,         // ||
        Not,        // !
        Xor,        // ^^
        BAnd,       // &
        BOr,        // |
        Lsh,        // <<
        Rsh,        // >>
        Assign,     // =
        Question,   // ?
        Colon,      // :
    };
    Type type;
    std::string val;
};
