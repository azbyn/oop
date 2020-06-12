#include "multime.h"
#include <complex>

void testInt() {
    Multime<int> a = { 1, 3, 2 };

    Multime<int> b = { 1, 1 };
    Multime<int> c = { 1 };
    Multime<int> d = { 1, 8 };
    Multime<int> o = { };

    std::cout << "A   = " << a << "\n";
    std::cout << "B   = " << b << "\n";
    std::cout << "C   = " << c << "\n";
    std::cout << "D   = " << c << "\n";
    std::cout << "A∩B = " << (a^b)<< "\n";
    std::cout << "A∪B = " << (a+b) << "\n";
    std::cout << "A\\B = " << (a-b) << "\n";
    std::cout << "B\\A = " << (b-a) << "\n";
    std::cout << "\n";

    std::cout << "B = C: " << (b==c) << "\n";
    std::cout << "C = A: " << (c==a) << "\n";
    std::cout << "C ⊆ A: " << (c<=a) << "\n";
    std::cout << "B ⊇ A: " << (b>=a) << "\n";
    std::cout << "C ⊂ A: " << (c<a) << "\n";
    std::cout << "C ⊂ B: " << (c<b) << "\n";
    std::cout << "A ⊃ C: " << (a>c) << "\n";
    std::cout << "D ⊂ A: " << (d<a) << "\n";
    std::cout << "A ⊂ D: " << (a<d) << "\n";
    std::cout << "A ≠ D: " << (a!=d) << "\n";

    std::cout << "\n";

    std::cout << "∅ = A: " << (o==a) << "\n";
    std::cout << "∅ = ∅: " << (o==o) << "\n";

    std::cout << "∅ ⊆ A: " << (o<=a) << "\n";
    std::cout << "∅ ⊆ ∅: " << (o<=o) << "\n";
    std::cout << "∅ ⊇ A: " << (o>=a) << "\n";
    std::cout << "∅ ⊂ A: " << (o<a) << "\n";
    std::cout << "∅ ⊃ A: " << (o>a) << "\n";
    std::cout << "∅ ⊂ ∅: " << (o<o) << "\n";
    std::cout << "∅ ≠ A: " << (o!=a) << "\n";
    std::cout << "∅ ≠ ∅: " << (o!=o) << "\n";

    std::cout << "\n";
    a ^= a;
    std::cout << "A ∩= A: " << a << "\n";

    a -= a;
    std::cout << "A \\= A: " << a << "\n";

    b += b;
    std::cout << "B ∪= B: " << b << "\n";


    b = b;
    std::cout << "B = B: " << b << "\n";
}
void testComplex() {
    using namespace std::complex_literals;

    Multime<std::complex<double>> a = { 1.0+3i, 3.0+1i, 2.0 };
    Multime<std::complex<double>> b = { 1.0+3i, 1.0+3i };
    Multime<std::complex<double>> c = { 1.0+3i };
    Multime<std::complex<double>> d = { 1.0+3i, 8 };
    Multime<std::complex<double>> o = { };

    std::cout << "A   = " << a << "\n";
    std::cout << "B   = " << b << "\n";
    std::cout << "C   = " << c << "\n";
    std::cout << "D   = " << c << "\n";
    std::cout << "A∩B = " << (a^b)<< "\n";
    std::cout << "A∪B = " << (a+b) << "\n";
    std::cout << "A\\B = " << (a-b) << "\n";
    std::cout << "B\\A = " << (b-a) << "\n";
    std::cout << "\n";

    std::cout << "B = C: " << (b==c) << "\n";
    std::cout << "C = A: " << (c==a) << "\n";
    std::cout << "C ⊆ A: " << (c<=a) << "\n";
    std::cout << "B ⊇ A: " << (b>=a) << "\n";
    std::cout << "C ⊂ A: " << (c<a) << "\n";
    std::cout << "C ⊂ B: " << (c<b) << "\n";
    std::cout << "A ⊃ C: " << (a>c) << "\n";
    std::cout << "D ⊂ A: " << (d<a) << "\n";
    std::cout << "A ⊂ D: " << (a<d) << "\n";
    std::cout << "A ≠ D: " << (a!=d) << "\n";

    std::cout << "\n";

    std::cout << "∅ = A: " << (o==a) << "\n";
    std::cout << "∅ = ∅: " << (o==o) << "\n";

    std::cout << "∅ ⊆ A: " << (o<=a) << "\n";
    std::cout << "∅ ⊆ ∅: " << (o<=o) << "\n";
    std::cout << "∅ ⊇ A: " << (o>=a) << "\n";
    std::cout << "∅ ⊂ A: " << (o<a) << "\n";
    std::cout << "∅ ⊃ A: " << (o>a) << "\n";
    std::cout << "∅ ⊂ ∅: " << (o<o) << "\n";
    std::cout << "∅ ≠ A: " << (o!=a) << "\n";
    std::cout << "∅ ≠ ∅: " << (o!=o) << "\n";

    std::cout << "\n";
    a ^= a;
    std::cout << "A ∩= A: " << a << "\n";

    a -= a;
    std::cout << "A \\= A: " << a << "\n";

    b += b;
    std::cout << "B ∪= B: " << b << "\n";


    b = b;
    std::cout << "B = B: " << b << "\n";
}

int main() {
    std::cout << "Int:\n";
    testInt();

    std::cout << "\n\nComplex:\n";
    testComplex();
    return 0;
}
