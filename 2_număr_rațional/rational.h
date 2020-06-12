#pragma once

#include <iostream>
#include <cmath>

class Rational {
    int64_t a, b;

public:
    constexpr int64_t denominator() const { return a; }
    constexpr int64_t numerator() const { return b; }

    Rational(int a);
    Rational(int64_t a = 0, int64_t b = 1);

    Rational(double d);

    Rational operator-() const;
    Rational operator+() const;

    friend Rational operator+(const Rational& l, const Rational& r);
    friend Rational operator-(const Rational& l, const Rational& r);
    friend Rational operator*(const Rational& l, const Rational& r);

    friend Rational operator/(const Rational& l, const Rational& r);

    Rational& operator+=(const Rational& r);
    Rational& operator-=(const Rational& r);
    Rational& operator/=(const Rational& r);
    Rational& operator*=(const Rational& r);


    friend bool operator==(const Rational& l, const Rational& r);
    friend bool operator!=(const Rational& l, const Rational& r);
    friend bool operator<=(const Rational& l, const Rational& r);
    friend bool operator< (const Rational& l, const Rational& r);
    friend bool operator> (const Rational& l, const Rational& r);
    friend bool operator>=(const Rational& l, const Rational& r);

    Rational reciprocal() const;

    explicit operator double() const;
    friend std::ostream& operator<<(std::ostream& s, const Rational& a);
    friend std::istream& operator>>(std::istream& s, Rational& a);

    static void tests();
};
