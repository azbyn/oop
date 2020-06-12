#pragma once

#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>

template<typename... Args>
inline std::string concat(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}


template<typename T>
class BinaryTree {
    template<typename V>
    class ManagedPtr {
        V* ptr = nullptr;
    public:
        ManagedPtr() {}
        template<typename... Args>
        ManagedPtr(Args&& ... args)
                : ptr(new V(std::forward<Args>(args)...)) {}
        ManagedPtr(const ManagedPtr& rhs) : ptr(new V(*rhs)) {}
        ManagedPtr(std::nullptr_t) : ptr(nullptr) {}
        ManagedPtr(ManagedPtr&& rhs) : ptr(std::exchange(rhs.ptr, nullptr)) {}
        ManagedPtr& operator=(const ManagedPtr& rhs) {
            if (!ptr) ptr = new ManagedPtr(*rhs);
            else {
                ptr->~V();
                *ptr = *rhs;
            }
            return *this;
        }
        ManagedPtr& operator=(ManagedPtr&& rhs) {
            delete ptr;
            ptr = std::exchange(rhs.ptr, nullptr);
            return *this;
        }

        ~ManagedPtr() {
            delete ptr;
        }

        friend bool operator !=(std::nullptr_t, const ManagedPtr& v) { return v.ptr != nullptr; }
        friend bool operator !=(const ManagedPtr& v, std::nullptr_t) { return v.ptr != nullptr; }
        friend bool operator ==(std::nullptr_t, const ManagedPtr& v) { return v.ptr == nullptr; }
        friend bool operator ==(const ManagedPtr& v, std::nullptr_t) { return v.ptr == nullptr; }

        V& operator*()  { return *ptr; }
        V* operator->() { return ptr; }
        V* operator&()  { return ptr; }
        const V& operator*() const  { return *ptr; }
        const V* operator&() const  { return ptr; }
        const V* operator->() const { return ptr; }
        operator bool() const { return ptr; }
    };

    struct Node {
        T val;
        ManagedPtr<Node> left;
        ManagedPtr<Node> right;

        explicit Node(const T& val)
                : val(val), left(nullptr), right(nullptr) {}
    };
    ManagedPtr<Node> root;

    static void print(std::ostream& s, const ManagedPtr<Node>& t,
                      size_t indent = 0) {
        for (size_t i = 0; i < indent * 2; ++i) s << ' ';
        if (t == nullptr) {
            s << "null\n";
            return;
        }
        s << t->val << "\n";
        if (t->left == nullptr && t->right == nullptr) return;
        print(s, t->left, indent + 1);
        print(s, t->right, indent + 1);
    }
public:
    BinaryTree(): root(nullptr) {}

    friend std::ostream& operator<<(std::ostream& s, const BinaryTree& t) {
        print(s, t.root);
        return s;
    }

    template <typename F>
    constexpr void visitPostorder(F f) const {
        auto impl = [&f] (auto& t, auto& impl) -> void {
            if (t->left) impl(t->left, impl);
            if (t->right) impl(t->right, impl);
            f(t->val);
        };
        if (root == nullptr) return;
        impl(root, impl);
    }
    template <typename F>
    constexpr void visitPreorder(F f) const {
        auto impl = [&f] (auto& t, auto& impl) -> void {
            f(t->val);
            if (t->left) impl(t->left, impl);
            if (t->right) impl(t->right, impl);
        };
        if (root == nullptr) return;
        impl(root, impl);
    }
    template <typename F>
    constexpr void visitInorder(F f) const {
        auto impl = [&f] (auto& t, auto& impl) -> void {
            if (t->left) impl(t->left, impl);
            f(t->val);
            if (t->right) impl(t->right, impl);
        };
        if (root == nullptr) return;
        impl(root, impl);
    }
    static BinaryTree fromFile(const char* path) {
        BinaryTree res;
        Parser p(path, res);
        return res;
    }

private:
    struct Parser {
        std::ifstream f;
        BinaryTree& res;
        size_t numNodes;
        size_t line = 1;
        Parser(const char* path, BinaryTree& res)
                : f(path, std::ios::in | std::ios::binary), res(res) {
            res.root = readNode();
        }
        template<typename Val>
        Val enforceVal(const char* msg) {
            eatSpace();
            Val res{};
            if (!(f >> res)) syntaxError(msg);
            return res;
        }
        ManagedPtr<Node> readNode() {
            if (checkChar(';')) return nullptr;

            ManagedPtr<Node> val{enforceVal<T>("Expected Node")};
            if (!checkChar(':')) return val;
            val->left = readNode();
            val->right = readNode();
            return val;
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
            syntaxError(
                concat("Expected '", msg, "', got '", char(f.get()), "'"));
        }
        void syntaxError(const char* msg) {
            throw std::runtime_error(
                concat("Syntax error on line ", line, ": ", msg));
        }
        constexpr static bool isEof(char c) {
            return c == std::char_traits<char>::eof();
        }
    };
};
