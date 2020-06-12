#pragma once
#include <iostream>
// #include "utils.h"

#include <utility>

template<typename T>
struct DoubleList {
    struct Node {
        T val;
        Node* next;
        Node* prev;
    };

    struct It {
        const Node* n;
        constexpr It(const Node* n) : n(n) {}
        constexpr It& operator++() { n = n->next; return *this; }
        constexpr It& operator--() { n = n->prev; return *this; }

        constexpr It operator++(int) { auto r = n; ++(*this); return r; }
        constexpr It operator--(int) { auto r = n; --(*this); return r; }

        constexpr auto& operator*() { return n->val; }
        constexpr auto& operator->() { return n->val; }
        constexpr bool operator==(const It& rhs) const { return n == rhs.n; }
        constexpr bool operator!=(const It& rhs) const { return n != rhs.n; }

        constexpr bool hasNext() const { return n->next; }
        constexpr bool hasPrev() const { return n->prev; }
    };

    Node* top;
    Node* bot;
    
    constexpr DoubleList(Node* top = nullptr, Node* bot = nullptr)
            : top(top), bot(bot) {}

    DoubleList(std::initializer_list<T> l) : DoubleList() {
        for (auto& val : l) emplace_back(val);
    }

    DoubleList(const DoubleList&) = delete;
    DoubleList& operator=(const DoubleList&) = delete;

    constexpr It begin() const { return top; }
    constexpr It end() const { return nullptr; }

    ~DoubleList() {
        while (top) {
            Node* n = top;
            top = top->next;
            delete n;
        }
    }
    DoubleList(DoubleList&& rhs) noexcept
        : top(std::exchange(rhs.top, nullptr)),
          bot(std::exchange(rhs.bot, nullptr)) {}
    DoubleList& operator=(DoubleList&& rhs) noexcept {
        this->~DoubleList();
        this->top = std::exchange(rhs.top, nullptr);
        this->bot = std::exchange(rhs.bot, nullptr);
    }

    T& front() { return top->val; }
    T& back() { return bot->val; }
    T pop_front() {
        assert(top, "Empty list");
        Node* n = top;
        top = top->next;
        if (top == nullptr) bot = nullptr;
        top->prev = nullptr;
        T res = n->val;
        delete n;
        return res;
    }

    T pop_back() {
        assert(bot, "Empty list");
        Node* n = bot;
        bot = bot->prev;
        if (bot == nullptr) bot = nullptr;
        bot->next = nullptr;
        T res = n->val;
        delete n;
        return res;
    }

    void push_front(const T& val) { emplace_front(val); }
    void push_front(T&& val) { emplace_front(std::move(val)); }

    template<typename... Args>
    void emplace_front(Args&& ... args) {
        Node* n = new Node{T(std::forward<Args>(args)...), top, nullptr};
        if (top == nullptr) bot = n;
        else top->prev = n;
        top = n;
    }

    void push_back(const T& val) { emplace_back(std::move(val)); }
    void push_back(T&& val) { emplace_back(std::move(val)); }

    template<typename... Args>
    void emplace_back(Args&& ... args) {
        Node* n = new Node{T(std::forward<Args>(args)...), nullptr, bot};
        if (top == nullptr) top = n;
        else bot->next = n;
        bot = n;
    }
    bool empty() const { return top == nullptr; }

    bool operator==(const DoubleList& rhs) {
        for (auto it = top, rit = rhs.top;
            it != nullptr;
            it = it->next, rit = rit->next) {
            if (it->val != rit->val) return false;
        }
        return true;
    }

    std::ostream& print(std::ostream& s,
                        std::string_view before = "",
                        std::string_view after = ", ",
                        std::string_view first = "{",
                        std::string_view last = "}") const {
        if (empty()) return s << first << last;
        s << first;
        const Node* it = top;
        while (it->next != nullptr) {
            s << before << it->val << after;
            it = it->next;
        }
        return s << before << it->val << last;
    }

    friend std::ostream& operator<<(std::ostream& s, const DoubleList& st) {
        if constexpr (std::is_same_v<T, char>) {
            return st.print(s, "", "","","");
        }
        else {
            // return st.print(s);
            return st.print(s, "\n  ", ", ", "{", "\n}");
        }
    }

    void remove(It n) {
        if (!n->prev) { pop_front(); return; }
        if (!n->next) { pop_back(); return; }
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

    It find(const T& val) {
        return find_if([&] (const T& rhs) { return rhs == val; });
    }

    // P is a predicate on T
    template<typename P>
    It find_if(P p) {
        auto n = top;
        for (; n; n = n->next) {
            if (p(n->val))
                return n;
        }
        return n;
    }
    void insert(It n, const T& val) {
        if (n == bot) { push_back(val); return; }
        Node* newN = new Node{ val, n->next, n};
        n->next->prev = newN;
        n->next = newN;
    }
    size_t size() const {
        auto it = top;
        size_t res = 0;
        while (it) { it = it->next; ++res; }
        return res;
    }
};

