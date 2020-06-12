#pragma once

#include <sstream>

template<typename... Args>
inline std::string concat(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}
template<char C = ' '>
struct Spacer {
    size_t cnt;
    explicit Spacer(size_t cnt) : cnt(cnt) {}
    friend std::ostream& operator << (std::ostream& s, Spacer sp) {
        for (size_t i = 0; i < sp.cnt; ++i) s << C;
        return s;
    }
};
