#pragma once

#include <array>
#include <sstream>
#include <string_view>

template<typename... Args>
std::string concat(Args&&... args) {
    std::stringstream ss;
    //only works in c++17
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}

class InvalidCnp : public std::invalid_argument {
public:
    InvalidCnp(std::string_view value, std::string_view why)
        : std::invalid_argument(concat("Invalid CNP '", value,"': ", why, ".")) {}
};

enum class Sex { Male, Female };

class Cnp {
    // digits stored as ints
    std::array<uint8_t, 13> val;

public:
    Cnp(std::string_view str);

    Sex getSex() const;
    unsigned getYear() const;
    unsigned getMonth() const;
    unsigned getDay() const;
    unsigned getJudetIndex() const;
    unsigned getNNN() const;

    friend std::ostream& operator<<(std::ostream& s, const Cnp& cnp);

    uint64_t substringAsInt(int start, int end) const;
};

