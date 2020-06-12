#pragma once

#include <iostream>
#include <utility>

template<typename T>
struct BinaryTree {
    template<typename V>
    class ManagedPtr {
        V* ptr = nullptr;
    public:
        ManagedPtr() {}
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
        }

        ~ManagedPtr() {
            delete ptr;
        }

        V& operator*() { return *ptr; }
        V& operator->() { return *ptr; }
        V* operator&() { return ptr; }
        const V& operator*() const { return *ptr; }
        const V* operator&() const { return ptr; }
        const V& operator->() const { return *ptr; }
    };
    struct Node {
        T val;
        ManagedPtr<Node> left;
        ManagedPtr<Node> right;

        Node(const T& val,
             ManagedPtr<Node> left = nullptr,
             ManagedPtr<Node> right = nullptr)
                : val(val), left(left), right(right) {}
    };
    ManagedPtr<Node> root;

private:
    static void print(std::ostream& s, const Node* t, size_t indent = 0) {
        for (size_t i = 0; i < indent * 2; ++i) std::cout << ' ';
        if (t == nullptr) {
            std::cout << "null\n";
            return;
        }
        std::cout << t->val << "\n";
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
    constexpr void visitPostOrder(F f) const {
        auto impl = [&f] (Node* t, auto& impl) -> void {
            if (t->left) impl(t->left, impl);
            if (t->right) impl(t->right, impl);
            f(t->val);
        };
        if (root == nullptr) return;
        impl(root, impl);
    }
    template <typename F>
    constexpr void visitPreorder(F f) const {
        auto impl = [&f] (Node* t, auto& impl) -> void {
            f(t->val);
            if (t->left) impl(t->left, impl);
            if (t->right) impl(t->right, impl);
        };
        if (root == nullptr) return;
        impl(root, impl);
    }
    template <typename F>
    constexpr void visitInorder(F f) const {
        auto impl = [&f] (Node* t, auto& impl) -> void {
            if (t->left) impl(t->left, impl);
            f(t->val);
            if (t->right) impl(t->right, impl);
        };
        if (root == nullptr) return;
        impl(root, impl);
    }
};
