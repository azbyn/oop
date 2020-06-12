#pragma once

#include "token.h"
#include "misc.h"

#include <cstring>
#include <vector>


struct TokenizerException : std::runtime_error {
    TokenizerException(const std::string& line, int pos, const std::string& msg)
            : std::runtime_error(concat(msg, ":\n", line, Spacer(pos), '^')) {}
};

class Tokenizer {
    const char* str;
    const char* end;
    std::string orgStr;

public:
    static Tokenizer Instance() { static Tokenizer i; return i; }

private:
    Tokenizer() {}



    // template<Token::Type Tt = TT::Eof, size_t Group = 0>
    // bool rpat(std::regex& regex) {
    //     std::cmatch m;

    //     std::cout << "match? " << str << "\n";
    //     if (std::regex_match(str, end, m, regex)) {
    //         std::cout << "match: \n";
    //         if constexpr (Tt != TT::Eof)
    //             result.emplace_back(Tt, m.str(Group));
    //         str += m.length();
    //         // str = str.substr(m.length());
    //         return true;
    //     }
    //     return false;
    // }

public:
    // returns true on success
    bool tryTokenize(const std::string& s, std::vector<Token>& res,
                     TokenizerException& err) {
        try {
            tokenize(s, res);
            // err = nullptr;
            return true;
        } catch (TokenizerException e) {
            err = e;
            //res = nullptr;
            return false;
        }
    }
private:
    bool whitespace() {
        bool res = isspace(*str);
        if (res) ++str;
        return res;
    }
    bool comment() {
        bool res = *str == '#';
        if (res) {
            ++str;
            for(;;) {
                if (isEof()) break;
                if (*str++ == '\n') break;
            }
        }
        return res;
    }
    bool int_(std::vector<Token>& result) {
        bool res = *str >= '0' && *str <= '9';
        if (res) {
            auto start = str;
            ++str;
            while (!isEof()) {
                if (*str < '0' || *str > '9') break;
                ++str;
            }
            result.emplace_back(TT::Int, std::string(start, str - start));
        }
        return res;
    }
    bool var(std::vector<Token>& result) {
        bool res = isalpha(*str) || *str == '*';
        if (res) {
            auto start = str;
            ++str;
            while (!isEof()) {
                if (!isalnum(*str) && *str != '_') break;
                ++str;
            }
            result.emplace_back(TT::Var, std::string(start, str - start));
        }
        return res;
    }
    bool isEof() {
        return (str >= end);
    }
public:
    // can throw TokenizerException
    void tokenize(const std::string& s, std::vector<Token>& result) {
        auto pat = [&](const char* s, Token::Type tt) {
            size_t n = strlen(s);
            if (strncmp(s, str, n) == 0) {//aka startsWith
                result.emplace_back(tt, s);
                // std::cout << "pat " << s << "\n";
                // str = str.substr(n);
                str += n;
                return true;
            }
            return false;
        };
        result.clear();
        //str = string.Join(" ", s.Split('\n', '\r'));
        orgStr = s;
        str = orgStr.data();// .begin();
        end = str + orgStr.size();
        // while (str.size() > 0) {
        while (!isEof()) {
            if (whitespace()) continue;
            if (comment()) continue;
            if (int_(result)) continue;
            if (var(result)) continue;
            if (pat(".", Token::Type::Dot)) continue;
            if (pat("\\", Token::Type::Lambda)) continue;
            if (pat("λ", Token::Type::Lambda)) continue;
            if (pat("(", Token::Type::LeftParen)) continue;
            if (pat(")", Token::Type::RightParen)) continue;
            if (pat("+", Token::Type::Plus)) continue;
            if (pat("-", Token::Type::Minus)) continue;
            if (pat("**", Token::Type::Pow)) continue;
            if (pat("^^", Token::Type::Xor)) continue;
            if (pat("^", Token::Type::Pow)) continue;
            if (pat("*", Token::Type::Mul)) continue;
            if (pat("/", Token::Type::Div)) continue;
            if (pat("%", Token::Type::Mod)) continue;
            if (pat("<<", Token::Type::Lsh)) continue;
            if (pat(">>", Token::Type::Rsh)) continue;
            if (pat("==", Token::Type::Eq)) continue;
            if (pat("!=", Token::Type::Ne)) continue;
            if (pat("<=", Token::Type::Le)) continue;
            if (pat(">=", Token::Type::Ge)) continue;
            if (pat("<", Token::Type::Lt)) continue;
            if (pat(">", Token::Type::Gt)) continue;
            if (pat("&&", Token::Type::And)) continue;
            if (pat("||", Token::Type::Or)) continue;
            if (pat("!", Token::Type::Not)) continue;
            if (pat("&", Token::Type::BAnd)) continue;
            if (pat("|", Token::Type::BOr)) continue;
            if (pat("=", Token::Type::Assign)) continue;
            if (pat("?", Token::Type::Question)) continue;
            if (pat(":", Token::Type::Colon)) continue;

            throw TokenizerException(orgStr, str - orgStr.data(),//.size(),
                                     concat("Invalid char '", str[0], '\''));
        }
    }
};
