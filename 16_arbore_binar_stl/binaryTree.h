#pragma once

#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>

template<typename... Args>
inline std::string concat(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}

template<typename T>
class BinaryTree {
    using Index = size_t;
    constexpr static Index Root = 0;

    std::vector<T> data;
    std::vector<Index> indices;

    constexpr bool isInvalid(Index i) const {
        return i >= data.size();
    }

    constexpr Index& getLeft(Index i)  { return indices[i*2]; }
    constexpr Index& getRight(Index i) { return indices[i*2+1]; }

    constexpr Index getLeft(Index i)  const { return indices[i*2]; }
    constexpr Index getRight(Index i) const { return indices[i*2+1]; }

    constexpr bool hasLeft(Index i)  const { return !isInvalid(getLeft(i)); }
    constexpr bool hasRight(Index i) const { return !isInvalid(getRight(i)); }

    constexpr const T& getValue(Index i) const { return data[i]; }
    constexpr T& getValue(Index i) { return data[i]; }

    template<size_t IndentSize = 2>
    void print(std::ostream& s, Index index = Root, size_t indent = 0) const {
        for (size_t i = 0; i < indent * IndentSize; ++i)
            s << ' ';
        if (isInvalid(index)) {
            s << "null\n";
            return;
        }
        s << getValue(index) << "\n";
        if (!hasLeft(index) && !hasRight(index)) return;
        print(s, getLeft(index), indent + 1);
        print(s, getRight(index), indent + 1);
    }

public:
    BinaryTree() {}

    friend std::ostream& operator<<(std::ostream& s, const BinaryTree& t) {
        t.print(s);

        // s << "(";
        // for(auto& a :t.data) s<<a << ", ";
        // s << ")";
        // s << "(";
        // for(auto& a :t.indices) s<< (int)a << ", ";
        // s << ")";
        return s;
    }

private:
    template <size_t When, typename F>
          requires (When >= 0 && When <= 2)
    constexpr void visitImpl(F f) const {
        auto impl = [this, &f] (Index i, auto& impl) -> void {
            if constexpr (When == 0) f(getValue(i));
            if (hasLeft(i)) impl(getLeft(i), impl);

            if constexpr (When == 1) f(getValue(i));
            if (hasRight(i)) impl(getRight(i), impl);

            if constexpr (When == 2) f(getValue(i));
        };

        if (isInvalid(Root)) return;
        impl(Root, impl);
    }

public:

    constexpr void visitPreOrder(auto f) const { visitImpl<0>(f); }
    constexpr void visitInOrder(auto f) const { visitImpl<1>(f); }
    constexpr void visitPostOrder(auto f) const { visitImpl<2>(f); }

    static BinaryTree fromFile(const char* path) {
        BinaryTree res;
        Parser p(path, res);
        return res;
    }

private:
    static constexpr Index InvalidIndex = std::numeric_limits<Index>::max();

    Index addNode(T&& val) {
        Index res = data.size();
        data.push_back(std::move(val));
        // 1 for the left + 1 for the right = 2
        indices.insert(indices.end(), 2, InvalidIndex);
        return res;
    }
    friend struct Parser;
    struct Parser {
        std::ifstream f;
        BinaryTree& res;
        size_t numNodes;
        size_t line = 1;

        Parser(const char* path, BinaryTree& res)
                : f(path, std::ios::in | std::ios::binary), res(res) {
            res.data.clear();
            res.indices.clear();
            readNode();
        }
        template<typename Val>
        Val enforceVal(const char* msg) {
            eatSpace();
            Val res{};
            if (!(f >> res)) syntaxError(msg);
            return res;
        }
        Index readNode(int level=0) {
            std::string idt = std::string(level*2, ' ')+"|";

            if (checkChar(';')) return res.InvalidIndex;
            auto node = res.addNode(enforceVal<T>("Expected Node"));
            if constexpr (std::is_same_v<T, std::string>) {
                if (res.getValue(node).back() == ':') {
                    res.getValue(node).pop_back();
                }
                else if (!checkChar(':')) return node;
            }
            else {
                if (!checkChar(':')) return node;
            }
            res.getLeft(node) = readNode(level+1);
            res.getRight(node) = readNode(level+1);

            return node;
        }
        //returns is eof
        bool eatSpace() {
            for (;;) {
                char c = f.get();
                if (isEof(c)) return true;
                if (c == '\n') {
                    ++line;
                }
                else if (!isspace(c)) {
                    f.unget();
                    break;
                }
            }
            return false;
        }

        bool checkChar(char c) {
            eatSpace();

            if (f.get() != c) {
                f.unget();
                return false;
            }
            return true;
        }
        void enforceKeyword(const char* msg) {
            eatSpace();
            const char* p = msg;
            while (*p) {
                if (f.get() != *p++) {
                    expectedErr(msg);
                    ///return false;
                }
            }
        }
        void expectedErr(const char* msg) {
            syntaxError(concat("Expected '", msg, "', got '", char(f.get()), "'"));
        }
        void syntaxError(const char* msg) {
            throw std::runtime_error(concat("Syntax error on line ", line, ": ", msg));
        }
        constexpr static bool isEof(char c) {
            return c == std::char_traits<char>::eof();
        }
    };
};
