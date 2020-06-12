#pragma once

#include <iostream>
#include <utility>

template<typename T>
concept Boolean = std::is_same_v<T, bool>;

template<typename T>
concept Comparable = requires(T a, T b) {
    { a == b } -> Boolean;
    { a != b } -> Boolean;
};
template<typename T>
concept Ordered = Comparable<T> && requires(T a, T b) {
    { a < b } -> Boolean;
    { a > b } -> Boolean;
    { a <= b } -> Boolean;
    { a >= b } -> Boolean;
};

template<Comparable T>
class Multime {
    T* val;
    size_t capacity;
    size_t n;

public:
    explicit Multime(size_t capacity);
    Multime();
    ~Multime();

    Multime(const Multime& m);
    Multime(Multime&& m);

    Multime& operator=(const Multime& m);
    Multime& operator=(Multime&& m);

    Multime(std::initializer_list<T> list);

private:
    constexpr T* begin() { return val; }
    constexpr T* end()   { return val+n; }

public:
    constexpr const T* begin() const { return val;}
    constexpr const T* end() const   { return val+n; }

    constexpr size_t size() const { return n; }
    constexpr size_t card() const { return n; }// cardinal

    Multime& operator +=(const T& val);
    Multime& operator -=(const T& val);

    Multime& operator +=(const Multime& rhs); // reuniune
    Multime& operator ^=(const Multime& rhs); // intersectie
    Multime& operator -=(const Multime& rhs); // diferenta

    Multime operator +(const Multime& rhs) const; // reuniune
    Multime operator ^(const Multime& rhs) const; // intersectie
    Multime operator -(const Multime& rhs) const; // diferenta

    template<Comparable U>friend bool operator==(const Multime<U>& a, const Multime<U>& b);
    template<Comparable U>friend bool operator!=(const Multime<U>& a, const Multime<U>& b);
    template<Comparable U>friend bool operator<=(const Multime<U>& a, const Multime<U>& b);// subseteq ⊆
    template<Comparable U>friend bool operator>=(const Multime<U>& a, const Multime<U>& b);// supseteq ⊇
    template<Comparable U>friend bool operator <(const Multime<U>& a, const Multime<U>& b);// subset ⊂
    template<Comparable U>friend bool operator >(const Multime<U>& a, const Multime<U>& b);// supset ⊃

    void add(const T& val);
    void remove(const T& val);
    constexpr bool find(const T& val) const;
    constexpr bool contains(const T& val) const {
        return find(val);
    }

    template<Comparable U>
    friend std::ostream& operator<<(std::ostream& s, const Multime<U>& val);
};

template<Comparable T>
Multime<T>::Multime() : Multime<T>(10) {}

template<Comparable T>
Multime<T>::Multime(size_t capacity)
        : val(new T[capacity]), capacity(capacity), n(0) {}

template<Comparable T>
Multime<T>::~Multime() {
    delete[] val;
}

template<Comparable T>
Multime<T>::Multime(std::initializer_list<T> list) : Multime() {
    for (auto el : list)
        add(el);
}

template<Comparable T>
Multime<T>::Multime(Multime<T>&& rhs)
        : val(std::exchange(rhs.val, nullptr)),
          capacity(std::exchange(rhs.capacity, 0)),
          n(std::exchange(rhs.n, 0)) {}

template<Comparable T>
Multime<T>::Multime(const Multime<T>& m) : Multime<T>(m.capacity) {
    this->n = m.n;
    for (size_t i = 0; i < m.n; ++i) {
        val[i] = m.val[i];
    }
}

template<Comparable T>
Multime<T>& Multime<T>::operator=(Multime<T>&& rhs) {
    if (rhs.val == val)
        return *this;

    this->~Multime();

    val = std::exchange(rhs.val, nullptr);
    capacity = std::exchange(rhs.capacity, 0);
    n = std::exchange(rhs.n, 0);

    return *this;
}

template<Comparable T>
Multime<T>& Multime<T>::operator=(const Multime<T>& m) {
    if (&m == this) return *this;
    if (this->capacity < m.capacity) {
        this->~Multime();
        capacity = m.capacity;
        val = new T[m.capacity];
    }
    this->n = m.n;
    for (size_t i = 0; i < m.n; ++i) {
        val[i] = m.val[i];
    }

    return *this;
}

template<Comparable T>
Multime<T>& Multime<T>::operator+=(const T& x) {
    add(x);
    return *this;
}
template<Comparable T>
Multime<T>& Multime<T>::operator-=(const T& x) {
    remove(x);
    return *this;
}
template<Comparable T>
void Multime<T>::add(const T& v) {
    if (n >= capacity) {
        T* tmp = new T[capacity*2];
        for (size_t i = 0; i < n; ++i)
            tmp[i] = std::move(val[i]);

        delete[] val;
        val = tmp;
        capacity = capacity*2;
    }

    if constexpr (Ordered<T>) {
        size_t i = 0;
        for (i = 0; i < n; i++) {
            if (val[i] > v) break;
            if (val[i] == v) return;
        }
        for (size_t j = n; j > i; j--)
            val[j] = val[j-1];
        val[i] = v;
    }
    else {
        if (contains(v)) return;
        val[n] = v;
    }

    n++;
}
template<Comparable T>
void Multime<T>::remove(const T& v) {
    for (size_t i = 0; i < n; i++) {
        if constexpr (Ordered<T>) {
            if (val[i] > v) break;
        }

        if (val[i] == v) {
            for (size_t j = i; j < n - 1; j++)
                val[j] = val[j + 1];
            n--;
        }
    }
}
template<Comparable T>
std::ostream& operator<<(std::ostream& out, const Multime<T>& M) {
    out << "{";
    if (M.n != 0) {
        for (size_t i = 0; i < M.n - 1; i++)
            out << M.val[i] << ", ";
        out << M.val[M.n-1];
    }
    out << "}";
    return out;
}

template <Comparable T>
Multime<T>& Multime<T>::operator+=(const Multime<T>& rhs) { // reuniune
    for (auto& v : rhs)
        add(v);
    return *this;
}

template<Comparable T>
constexpr bool Multime<T>::find(const T& x) const {
    for (size_t i = 0; i < n; ++i) {
        if constexpr (Ordered<T>) {
            if (val[i] > x) break;
        }
        if (val[i] == x) return true;
    }
    return false;
}

template <Comparable T>
Multime<T>& Multime<T>::operator^=(const Multime<T>& rhs) { // intersectie
    constexpr auto removeAt = [](T* p, size_t i, size_t& n) {
        for (size_t j = i; j < n - 1; ++j)
            p[j] = p[j + 1];
        --n;
    };
    for (size_t i = 0; i < n; ++i) {
        if (!rhs.find(val[i])) {
            removeAt(val, i, n);
            //improveable
            --i;
        }
    }
    return *this;
}
template <Comparable T>
Multime<T>& Multime<T>::operator-=(const Multime<T>& rhs) { // diferenta
    for (size_t i = 0; i < n; ++i) {
        if (rhs.find(val[i])) {
            //improveable
            remove(val[i]);
            --i;
        }
    }
    return *this;
}
template <Comparable T>
Multime<T> Multime<T>::operator+(const Multime<T>& rhs) const { // reuniune
    Multime<T> res(*this);
    return res += rhs;
}
template <Comparable T>
Multime<T> Multime<T>::operator^(const Multime<T>& rhs) const { // intersectie
    Multime<T> res(*this);
    return res ^= rhs;
}
template <Comparable T>
Multime<T> Multime<T>::operator-(const Multime<T>& rhs) const { // diferenta
    Multime res(*this);
    return res -= rhs;
}

template <Comparable T>
bool operator==(const Multime<T>& a, const Multime<T>& b) {
    return a <= b && b <= a;
}
template <Comparable T>
bool operator!=(const Multime<T>& a, const Multime<T>& b) {
    return (!(a <= b) || !(b <= a));
}

template <Comparable T>
bool operator<=(const Multime<T>& A, const Multime<T>& B) { // subseteq ⊆
    for (auto a : A) {
        if (!B.find(a))
            return false;
    }
    return true;
}

template <Comparable T>
bool operator>=(const Multime<T>& a, const Multime<T>& b) { // supseteq ⊇
    return b <= a;
}
template <Comparable T>
bool operator<(const Multime<T>& a, const Multime<T>& b) { // subset  ⊂
    return a <= b && !(b <= a);
}
template <Comparable T>
bool operator>(const Multime<T>& a, const Multime<T>& b) { // supset ⊃
    return b < a;
}
