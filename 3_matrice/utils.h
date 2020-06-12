#pragma once

#include <stdexcept>
#include <iostream>

inline void assert(bool cond, const char* msg) {
    if (!cond) throw std::logic_error(msg);
}

constexpr void getSize(std::initializer_list<
                       std::initializer_list<double>> list,
                       int& s1, int& s2) {
    constexpr int invalid = -1;
    s1 = 0;
    s2 = invalid;

    for (const auto& l: list) {
        int i = l.size();
        ++s1;
        if (s2 != invalid && i != s2) {
            throw std::logic_error("Fixed line size expected");
        }
        s2 = i;
    }
}
inline size_t readSize(const char* name, int a = 1, int b = 256) {
    int res;
    do {
        std::cout << name << ": ";
        std::cin >> res;
    } while (res < a || res > b);
    return res;
}
