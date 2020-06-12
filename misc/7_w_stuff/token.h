#pragma once

#include <string>

struct Token {
    enum class Type {
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

    Token(Type t, const char* val) :type(t), val(val) {}
    Token(Type t, const std::string& val) :type(t), val(val) {}
    Token(Type t, std::string&& val) :type(t), val(std::move(val)) {}

    friend std::string_view toString(Type tt) {
        switch (tt) {
        case Type::Eof:        return "eof";
        case Type::Int:        return "int";
        case Type::Var:        return "var";
        case Type::LeftParen:  return "(";
        case Type::RightParen: return "(";
        case Type::Lambda:     return "λ";
        case Type::Dot:        return ".";
        case Type::Plus:       return "+";
        case Type::Minus:      return "-";
        case Type::Div:        return "/";
        case Type::Mul:        return "*";
        case Type::Mod:        return "%";
        case Type::Pow:        return "^";
        case Type::Eq:         return "==";
        case Type::Ne:         return "!=";
        case Type::Lt:         return "<=";
        case Type::Gt:         return ">=";
        case Type::Le:         return "<";
        case Type::Ge:         return ">";
        case Type::And:        return "&&";
        case Type::Or:         return "||";
        case Type::Not:        return "!";
        case Type::Xor:        return "^^";
        case Type::BAnd:       return "&";
        case Type::BOr:        return "|";
        case Type::Lsh:        return "<<";
        case Type::Rsh:        return ">>";
        case Type::Assign:     return "=";
        case Type::Question:   return "?";
        case Type::Colon:      return ":";
        }
        return "???";
    }
};

using TT = Token::Type;
