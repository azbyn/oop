#include "rational.h"

#include <iomanip>

int main() {
    Rational::tests();
    Rational a = 3./2, b = 3;

    std::cout << b << "\n";
    std::cout << a << "\n";
    std::cout << (a + b) << "\n";
    std::cout << (a - b) << "\n";
    std::cout << (a * b) << "\n";
    std::cout << (a / b) << "\n";
    Rational pi = M_PI;
    std::cout << std::setprecision(16);
    std::cout << "Fraction: " << pi << "\n"
              << "Rational: "<< double(pi) << "\n"
              << "Double:   "<<  M_PI << "\n";
}
