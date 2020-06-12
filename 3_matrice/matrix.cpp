#include "matrix.h"
#include "utils.h"

#include <cmath>
#include <cstring>
#include <utility>

Matrix::Matrix(int m, int n) : m(m), n(n) { data = new double[m*n]; }

Matrix::Matrix(std::initializer_list<std::initializer_list<double>> list) {
    getSize(list, m, n);
    data = new double[m*n];

    auto it = begin();
    for (const auto& l : list) {
        for (const auto& v : l) *(it++) = v;
    }
}
Matrix::Matrix(Matrix&& rhs) noexcept :
        m(rhs.m), n(rhs.n),
        data(std::exchange(rhs.data, nullptr)) {}

Matrix::Matrix(const Matrix& rhs) : Matrix(rhs.m, rhs.n) {
    std::memcpy(data, rhs.data, sizeof(double) * m * n);
}

Matrix& Matrix::operator=(const Matrix& rhs) {
    setSize(rhs.m, rhs.n);
    std::memcpy(data, rhs.data, sizeof(double) *m*n);
    return *this;
}
Matrix& Matrix::operator=(Matrix&& rhs) noexcept {
    this->~Matrix();
    m = rhs.m;
    n = rhs.n;
    data = std::exchange(rhs.data, nullptr);

    return *this;
}

Matrix::~Matrix() noexcept {
    delete[] data;
}

double& Matrix::at(int i, int j) {
    assert(i < m && j < n, "out of range");
    return data[i * n + j];
}
double Matrix::at(int i, int j) const {
    assert(i < m && j < n, "out of range");
    return data[i * n + j];
}

double* Matrix::begin() { return data; }
const double* Matrix::begin() const { return data; }
double* Matrix::end() { return data + m*n; }
const double* Matrix::end() const { return data + m*n; }

void Matrix::setSize(int m, int n) {
    if (this->m == m && this->m == n) return;
    if (this->m * this->n >= m * n) {
        this->m = m;
        this->n = n;
    }
    *this = Matrix(m, n);
}

Matrix Matrix::read() {
    Matrix res(readSize("m"), readSize("n"));
    for (auto& i : res)
        std::cin >> i;
    return res;
}
std::ostream& operator<<(std::ostream& s, const Matrix& m) {
    s << "Matrix " << m.m << "x" << m.n << " {\n";
    for (int i = 0; i < m.m; ++i) {
        for (int j = 0; j < m.n; ++j) {
            s << m.at(i, j) << " ";
        }
        s << "\n";
    }
    return s<< "}\n";
}


Matrix& Matrix::add(const Matrix& a, const Matrix& b, Matrix& res) {
    assert(a.m == b.m && a.n == b.n, "Sizes don't match, can't add");
    res.setSize(a.m, a.n);
    for (int i = 0; i < a.m; ++i)
        for (int j = 0; j < a.n; ++j)
            res.at(i, j) = a.at(i, j) + b.at(i, j);
    return res;
}

Matrix& Matrix::mul(double a, const Matrix& b, Matrix& res) {
    res.setSize(b.m, b.n);

    for (int i = 0; i < res.m; ++i)
        for (int j = 0; j < res.n; ++j)
            res.at(i, j) = a * b.at(i, j);
    return res;
}

Matrix& Matrix::neg(const Matrix& a, Matrix& res) {
    return mul(-1, a, res);
}
Matrix& Matrix::sub(const Matrix& a, const Matrix& b, Matrix& res) {
    return add(a, neg(b, res), res);
}
Matrix& Matrix::mul(const Matrix& a, const Matrix& b, Matrix& res) {
    assert(a.n == b.m, "Sizes don't match, can't multiply");
    assert(a.data != res.data && b.data != res.data,
           "Can't multiply inplace");
    res.setSize(a.m, b.n);

    for (int i = 0; i < res.m; ++i)
        for (int j = 0; j < res.n; ++j) {
            res.at(i, j) = 0;
            for (int k = 0; k < a.n; ++k)
                res.at(i, j) += a.at(i, k) * b.at(k, j);
        }
    return res;
}

Matrix& Matrix::getTranspose(Matrix& res) const {
    assert(data != res.data, "Can't transpose inplace");
    res.setSize(n, m);

    for (int i = 0; i < res.m; ++i)
        for (int j = 0; j < res.n; ++j)
            res.at(i, j) = at(j, i);
    return res;
}

Matrix Matrix::getTranspose() const { Matrix res; return getTranspose(res); }

Matrix operator+(const Matrix& a, const Matrix& b) { Matrix res; return Matrix::add(a, b, res); }
Matrix operator-(const Matrix& a, const Matrix& b) { Matrix res; return Matrix::sub(a, b, res); }
Matrix operator*(const Matrix& a, const Matrix& b) { Matrix res; return Matrix::mul(a, b, res); }
Matrix operator*(const Matrix& a, double b)        { Matrix res; return Matrix::mul(b, a, res); }
Matrix operator*(double a, const Matrix& b)        { Matrix res; return Matrix::mul(a, b, res); }

Matrix& Matrix::operator+=(const Matrix& m) { return Matrix::add(*this, m, *this); }
Matrix& Matrix::operator-=(const Matrix& m) { return Matrix::sub(*this, m, *this); }
Matrix& Matrix::operator*=(double d)        { return Matrix::mul(d, *this, *this); }
Matrix& Matrix::operator*=(const Matrix& m) {
    Matrix res(*this);
    Matrix::mul(*this, m, res);
    return *this = res;
}

Matrix Matrix::operator-() const { Matrix res; return neg(*this, res); }
const Matrix& Matrix::operator+() const { return *this; }
