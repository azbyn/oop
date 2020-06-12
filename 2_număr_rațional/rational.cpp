#include "rational.h"


#include <cfloat>
#include <iomanip>
#include <random>

static Rational doubleToRationalImplementation(double d);

Rational::Rational(int a) : Rational(a, 1) {}
Rational::Rational(int64_t a, int64_t b) : a(a), b(b) {}

Rational::Rational(double d)
        : Rational(doubleToRationalImplementation(std::abs(d))) {
    if (d < 0) {
        a = -a;
    }
}

Rational Rational::operator-() const { return Rational(-a, b); }
Rational Rational::operator+() const { return Rational(a, b); }

Rational operator+(const Rational& l, const Rational& r) {
    return Rational(l.a * r.b + l.b * r.a, l.b * r.b);
}

Rational operator-(const Rational& l, const Rational& r) {
    return Rational(l.a * r.b - l.b * r.a, l.b * r.b);
}

Rational operator*(const Rational& l, const Rational& r) {
    return Rational(l.a * r.a, l.b * r.b);
}

Rational operator/(const Rational& l, const Rational& r) {
    return Rational(l.a * r.b, l.b * r.a);
}

Rational& Rational::operator+=(const Rational& r) { return *this = *this+r; }
Rational& Rational::operator-=(const Rational& r) { return *this = *this-r; }
Rational& Rational::operator/=(const Rational& r) { return *this = *this/r; }
Rational& Rational::operator*=(const Rational& r) { return *this = *this*r; }

bool operator==(const Rational& l, const Rational& r) { return l.a*r.b == l.b*r.a; }
bool operator!=(const Rational& l, const Rational& r) { return l.a*r.b != l.b*r.a; }
bool operator<=(const Rational& l, const Rational& r) { return l.a*r.b <= l.b*r.a; }
bool operator< (const Rational& l, const Rational& r) { return l.a*r.b <  l.b*r.a; }
bool operator> (const Rational& l, const Rational& r) { return l.a*r.b >  l.b*r.a; }
bool operator>=(const Rational& l, const Rational& r) { return l.a*r.b >= l.b*r.a; }

Rational Rational::reciprocal() const { return Rational(b, a); }
Rational::operator double() const { return double(a) / b; }

// only works for positive numbers
//
// scale is an aproximation for how big the denominator is
// we stop when the scale (and thus the denominator) is bigger than MAX so that we don't overflow
//
// we can't really say "if (iteration > something) return...;"
// because bigger numbers would be reach that threshold faster
static constexpr int64_t MAX = 1L <<32;// 32;
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
            // std::cout << "UNSAFE MULTIPLICATION\n";
            return 1;
        }
        scale *= i;
    }
    // std::cout << "iter " << " (" << d << ") :" << i << " den:"<< scale <<"\n"; 
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

static Rational doubleToRationalImplementation(double d) {
    if (d > MAX) return (int64_t) d;
    return doubleToRationalImplementation(d, 1);
}


std::ostream& operator<<(std::ostream& o, const Rational& a) {
    return o  << a.a << "/" << a.b;
}

std::istream& operator>>(std::istream& s, Rational& a) {
    s >> a.a;
    if (s.fail()) {
        throw std::logic_error("Invalid Rational string");
    }

    char c;
    for (;s>> c;) {
        if (c == ' ') continue;
        if (c == '/') {
            s >> a.b;
            if (s.fail()) {
                throw std::logic_error("Invalid Rational string");
            }
        }
    }
    return s;
}


/// TESTS

static void testExact(int64_t a, int64_t b) {
    Rational r = double(a) / b;
    Rational expected(a, b);
    if (r != expected) {
        double delta = std::abs(double(expected) - double(r));
        if (delta < 1e-5) {
            //std::cerr << "Test failed because double was not accurate enough:\n";
            return;
        }
        std::cerr<< "\n";
        std::cerr << "ERROR! Expected "<< expected << ", got " << r << "!\n";
        std::cerr << "\tdelta: " << delta << "\n";
    }
}
static bool testEq(Rational r, double d, double eps, const char* str = "") {
    auto delta = std::abs(double(r) - d);
    if (delta > eps) {
        std::cerr << "\n";
        std::cerr << std::setprecision(6);
        std::cerr << "ERROR "<< str <<"! Delta>eps: "<< delta << " > "<< eps<<"\n";
        std::cerr << std::setprecision(20);
        std::cerr << "\tGot "<< r << " = " << double(r) <<
            ", expected: "<< d<<"\n";
        return false;
    }
    else {
        return true;
    }
}
static void testEps(double d, double eps) {
    testEq(d, d, eps);
}
static bool testOps(Rational a, Rational b, double eps = 1e-5) {
    return testEq(a + b, double(a) + double(b), eps, "+") &
           testEq(a - b, double(a) - double(b), eps, "-") &
           testEq(a * b, double(a) * double(b), eps, "*") &
           testEq(a / b, double(a) / double(b), eps, "/");
}
static Rational fromString(const char* s) {
    std::istringstream ss(s);
    Rational r;
    ss >> r;
    return r;
}
static void testFromString(const char* s, int64_t a, int64_t b) {
    auto res = fromString(s);
    if (res.denominator() != a || res.numerator() != b)
        std::cerr << "ERROR@ testFromString: expected " << Rational(a, b)
                  << " got " << res << "\n";
}
template<typename ExceptionType, typename F>
static void testThrows(F f) {
    try {
        f();
    }
    catch (const ExceptionType&) {
        return;
    }
    std::cerr << "Expected exception, got nothing\n";
}
static void testOperatorIn() {
    testFromString("42/23", 42, 23);
    testFromString("42/1", 42, 1);
    testFromString("-12/1", -12, 1);
    testFromString("12/-1", 12, -1);
    testFromString("12", 12, 1);
    testThrows<std::logic_error>([] { fromString("12/"); });
    testThrows<std::logic_error>([] { fromString("12/-"); });
    testThrows<std::logic_error>([] { fromString("12/-adsjk"); });
    testThrows<std::logic_error>([] { fromString("adsjk"); });
    testThrows<std::logic_error>([] { fromString(""); });
}
void Rational::tests() {
    testOperatorIn();
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
    testEps(1e-7, DBL_EPSILON);
    testEps(M_PI, 1e-10);
    testEps(M_PI*100, 1e-10);
    testEps(1e4+M_PI, 1e-4);
    testEps(1e4+M_PI*100, 1);
    testEps(1e8+M_PI, 1);
    testEps(1e14+M_PI, 1);

    std::uniform_real_distribution<double> fdis(100, 400);
    for (int i = 0; i < 10000; ++i) {
        testEps(fdis(gen), 1e-2);
    }


    for (int i = 0; i < 10; ++i) {
        Rational a(dis(gen), dis(gen));
        Rational b(dis(gen), dis(gen));
        if (!testOps(a, b, 1e-4)) {
            std::cerr << "a = "<< a << " = "<< double(a) << "\n";
            std::cerr << "b = "<< b << " = " << double(b) << "\n";
        }
    }
}
