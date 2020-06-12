#pragma once


#include <iostream>

struct Matrix {
    int m, n;
    double *data;

    constexpr Matrix() : m(0), n(0), data(nullptr) {}
    Matrix(int m, int n);
    Matrix(std::initializer_list<std::initializer_list<double>> list);

    Matrix(const Matrix& rhs);
    Matrix(Matrix&& rhs) noexcept;

    Matrix& operator=(const Matrix& rhs);
    Matrix& operator=(Matrix&& rhs) noexcept;

    ~Matrix() noexcept;

    double& at(int i, int j);
    double at(int i, int j) const;

    double* begin();
    const double* begin() const;
    double* end();
    const double* end() const;

    void setSize(int m, int n);

    static Matrix read();

    friend std::ostream& operator<<(std::ostream& s, const Matrix& m);

    // allocates a new object everytime, not great for efficiency
    friend Matrix operator+(const Matrix& a, const Matrix& b);
    friend Matrix operator-(const Matrix& a, const Matrix& b);
    friend Matrix operator*(const Matrix& a, const Matrix& b);
    friend Matrix operator*(const Matrix& a, double b);
    friend Matrix operator*(double a, const Matrix& b);

    Matrix& operator+=(const Matrix& m);
    Matrix& operator-=(const Matrix& m);
    Matrix& operator*=(const Matrix& m);
    Matrix& operator*=(double d);

    Matrix operator-() const;
    const Matrix& operator+() const;

    //all of these return res
    static Matrix& add(const Matrix& a, const Matrix& b, Matrix& res);
    static Matrix& mul(double a, const Matrix& b, Matrix& res);
    static Matrix& neg(const Matrix& a, Matrix& res);
    static Matrix& sub(const Matrix& a, const Matrix& b, Matrix& res);
    static Matrix& mul(const Matrix& a, const Matrix& b, Matrix& res);
    //doesn't work inplace
    Matrix& getTranspose(Matrix& res) const;

    //doesn't care about inplace, because we allocate a new array
    Matrix getTranspose() const;
};
