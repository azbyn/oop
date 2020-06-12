#include "matrix.h"

#include <iomanip>

int main() {
    Matrix a = {{1, 3}, {3, 4}};
    Matrix b = {{1, 0}, {0, 1}};

    Matrix c = {{1, 3, 4}, {2, 3, 3}, {1, 2, 3}};

    std::cout << c << "\n";
    std::cout << c.getTranspose() << "\n";

    Matrix::add(a, b, c);
    std::cout << c << "\n";


    std::cout << a << "\n";
    std::cout << (a * a) << "\n";
    std::cout << (a * a) << "\n";
    std::cout << a << "\n";
    a *= a;
    std::cout << a << "\n";

    return 0;
}
