#pragma once

#include "token.h"
#include "misc.h"

#include <stdexcept>
#include <span>

using Pos = int;
struct PosException : std::runtime_error {
    Pos pos;
    PosException(Pos pos, std::span<Token> tokens, std::string_view msg)
            : std::runtime_error(niceString(tokens, msg)), pos(pos) {}

private:
    std::string niceString(std::span<Token> tokens, std::string_view msg) {
        std::string res = "";
        int spaceCount = 0;
        int i = 0;
        for (auto& t : tokens) {
            size_t len = t.val.size();
            res += t.val;

            if (t.type == Token::Type::Var) {
                len += 1;
                res += ' ';
            }
            if (i++ < pos)
                spaceCount += len;
        }
        return concat(msg, "\n", res, "\n", Spacer(spaceCount), "^");
    }
};
