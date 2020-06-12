#include <iostream>
#include <cfloat>
#include <iomanip>
#include <cmath>
#include <random>


class Rational {
    using T = int64_t;
    T a, b;
public:
    Rational(int a) : Rational(a, 1) {}
    Rational(T a=0, T b=1) : a(a), b(b) {}


    Rational(double d) : Rational(doubleToRationalImplementation(std::abs(d))) {
        if (d < 0) {
            a = -a;
        }
    }

    Rational operator-() const { return Rational(-a, b); }
    Rational operator+() const { return Rational(a, b); }

    friend Rational operator+(const Rational& l, const Rational& r) {
        return Rational(l.a*r.b +l.b*r.a, l.b*r.b);
    }

    friend Rational operator-(const Rational& l, const Rational& r) {
        return Rational(l.a*r.b -l.b*r.a, l.b*r.b);
    }

    friend Rational operator*(const Rational& l, const Rational& r) {
        return Rational(l.a*r.a, l.b*r.b);
    }

    friend Rational operator/(const Rational& l, const Rational& r) {
        return Rational(l.a*r.b, l.b*r.a);
    }

    Rational& operator+=(const Rational& r) { new (this) Rational(*this+r); return *this; }
    Rational& operator-=(const Rational& r) { new (this) Rational(*this-r); return *this; }
    Rational& operator/=(const Rational& r) { new (this) Rational(*this/r); return *this; }
    Rational& operator*=(const Rational& r) { new (this) Rational(*this*r); return *this; }


    friend bool operator==(const Rational& l, const Rational& r) { return l.a*r.b == l.b*r.a; }
    friend bool operator!=(const Rational& l, const Rational& r) { return l.a*r.b != l.b*r.a; }
    friend bool operator<=(const Rational& l, const Rational& r) { return l.a*r.b <= l.b*r.a; }
    friend bool operator< (const Rational& l, const Rational& r) { return l.a*r.b <  l.b*r.a; }
    friend bool operator> (const Rational& l, const Rational& r) { return l.a*r.b >  l.b*r.a; }
    friend bool operator>=(const Rational& l, const Rational& r) { return l.a*r.b >= l.b*r.a; }

    Rational reciprocal() const { return Rational(b, a); }

private:
    //only works for positive numbers
    //
    // scale is an aproximation for how big the denominator is
    // we stop when the scale (and thus the denominator) is bigger than MAX so that we don't overflow
    //
    // we can't really say "if (iteration > something) return...;"
    // because for bigger numbers that threshold would be reached faster
    static constexpr int64_t MAX = 1L << 32;
    static Rational doubleToRationalImplementation(double d) {
        if (d > MAX) return (int64_t) d;
        return doubleToRationalImplementation(d, 1);
    }
    static Rational doubleToRationalImplementation(double d, int64_t scale) {
        int64_t i = (int64_t)d;
        Rational r(i, 1);
        d -= i;
        if (d < 1.0 / MAX){
            return r;
        }
        if (i > MAX) return r;
        if (i != 0) {
            if (i > MAX / scale) {
                //std::cout << "UNSAFE MULTIPLICATION\n";
                return 0;
            }
            scale *= i;
        }
        //std::cout << "iter " << " (" << d << ") :" << i << " den:"<< scale <<"\n"; 
        // if (scale == 0) {
        //     std::cout << "blyat\n";
        //     return r;
        // }
        /*if (d < 1.0 / MAX || uint64_t(scale) > MAX) {
            std::cout << "overflow? " <<  r << ", den = " << scale <<"\n";
            return r;
        }*/
        auto b2 = doubleToRationalImplementation(1.0 / d, scale).reciprocal();
        //std::cout << "iter " << 0<< ": " << r << "+" << b2 << ", den = " << scale <<"\n";
        return r + b2;
    }
public:
    explicit operator double() const { return double(a) / b; }
    friend std::ostream& operator<<(std::ostream& o, const Rational& a) {
        return o  << a.a << "/" << a.b;
    }

    static void testExact(int64_t a, int64_t b) {
        Rational r = double(a) / b;
        Rational expected(a, b);
        if (r != expected) {
            double delta = std::abs(double(expected) - double(r));
            if (delta < 1e-5/* DBL_EPSILON*/) {
                //std::cerr << "Test failed because double was not accurate enough:\n";
                return;
            }
            std::cerr<< "\n";
            std::cerr << "ERROR! Expected "<< expected << ", got " << r << "!\n";
            std::cerr << "\tdelta: " << delta << "\n";
        } else {
            // std::cout << "success "<< expected << "\n";
        }
    }

    static bool testEq(Rational r, double d, double eps, const char* str = "") {
        auto delta = std::abs(double(r) - d);
        if (delta > eps) {
            std::cerr<< "\n";
            std::cerr << std::setprecision(6);
            std::cerr << "ERROR "<< str <<"! Delta>eps: "<< delta << " > "<< eps<<"\n";
            std::cerr << std::setprecision(20);
            std::cerr << "\tGot "<< r << " = " << double(r) <<
                ", expected: "<< d<<"\n";
            return false;
        } else {
            //std::cout << "success "<< d << "\n";
            return true;
        }
    }
    static void testEps(double d, double eps) {
        testEq(d, d, eps);
    //     Rational r = d;
    //     auto delta = std::abs(double(r) - d);
    //     if (delta > eps) {
    //         std::cerr<< "\n";
    //         std::cerr << std::setprecision(6);
    //         std::cerr << "ERROR! Delta>eps: "<< delta << " > "<< eps<<"\n";
    //         std::cerr << std::setprecision(20);
    //         std::cerr << "\tGot "<< r << " = " << double(r) <<
    //             ", expected: "<< d<<"\n";
    //     } else {
    //         //std::cout << "success "<< d << "\n";
    //     }
    }

    static bool testOps(Rational a, Rational b, double eps = 1e-5) {
        return testEq(a+b, double(a)+double(b), eps, "+") &
            testEq(a-b, double(a)-double(b), eps, "-") &
            testEq(a*b, double(a)*double(b), eps, "*") &
            testEq(a/b, double(a)/double(b), eps, "/");
    }
    static void tests() {
        testExact(3, 72);
        testExact(4, 3);
        testExact(231312, 6546);
        testExact(2017, 6833);
        testExact(891, 4375);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(1, 1000);
        for (int i = 0; i < 10000; ++i) {
            uint64_t a = dis(gen);
            uint64_t b = dis(gen);
            testExact(a, b);
            testExact(a, 1);
        }
        testEps(1e-8, DBL_EPSILON);
        testEps(M_PI, 1e-10);
        testEps(M_PI*100, 1e-10);
        testEps(1e4+M_PI, 1e-10);
        testEps(1e4+M_PI*100, 1);
        testEps(1e8+M_PI, 1);
        testEps(1e14+M_PI, 1);

        std::uniform_real_distribution<double> fdis(0, 1000);
        for (int i = 0; i < 10000; ++i) {
            testEps(fdis(gen), 1e-5);
        }


        for (int i = 0; i < 10; ++i) {
            Rational a = fdis(gen);
            Rational b = fdis(gen);
            if (!testOps(a, b, 1e-5)) {
                std::cerr << "a = "<< a << " = "<< double(a) << "\n";
                std::cerr << "b = "<< b << " = " << double(b) << "\n";
            }
        }

        // auto og = 2017./6833;// 1e-12;//+M_PI*10;
    }
};

int main() {
    //todo: drop precision
    Rational::tests();
    Rational a = 3./2, b = 3;

    std::cout << (b)<<"\n";
    std::cout << (a)<<"\n";
    std::cout << (a + b)<<"\n";
    std::cout << (a - b)<<"\n";
    std::cout << (a * b)<<"\n";
    std::cout << (a / b)<<"\n";
    // auto og = 2017./6833;// 1e-12;//+M_PI*10;
    // auto i = (int64_t) og;
    // std::cout << i << "\n";
    // std::cout << og << "\n";
    // Rational r = og;
    // std::cout << std::setprecision(25) << r <<"\n";// double(7)/24);

    // std::cout << og << "\n";
    // std::cout << (double) r << "\n";
}
