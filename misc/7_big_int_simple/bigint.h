#pragma once

#include <stdint.h>

#include <iostream>
#include <utility>

#include <type_traits>

struct BigInt;

template <typename T>
concept Integral = std::is_integral_v<T>;

//BigInt gets passed as const BigInt&
//https://godbolt.org/z/2vtHxA
template <typename T>
concept IntegralOrBig = std::is_integral_v<T> || std::is_same_v<T, BigInt>;

template <typename T>
concept UnsignedOrBig = (std::is_integral_v<T> && std::is_unsigned_v<T>)
    || std::is_same_v<T, BigInt>;


// numbers are stored in base 256 little-endian
struct BigInt {
    using byte = uint8_t;
    struct Error : public std::runtime_error {
        Error(const char* msg) : std::runtime_error(msg) {}
    };
private:
    enum class Sign : int8_t {
        Negative = -1,
        Zero = 0,
        Positive = 1,
    };

    friend constexpr Sign operator-(Sign sgn) { return Sign(-int(sgn)); }
    friend constexpr Sign operator*(Sign a, Sign b) {
        return (Sign) (int(a)* int(b));
    }

    friend constexpr int operator<=>(Sign a, int b) {
        return int(a) - b;
    }

    byte* _begin;
    size_t size;
    // /usr/include/c++/9.3.0/bits/basic_string.h:173
    static constexpr size_t SmallBufferSize = 8;
    //2 * sizeof(byte*) - sizeof(Sign);
    union {
        byte buffer[SmallBufferSize];
        size_t capacity;
    };
    // we could do something like a function returning _begin == buffer,
    // but we already waste memory for the sign, and because of alignment
    // one more byte doesn't matter
    // bool isUsingBuffer;
    Sign sign;

    constexpr bool isUsingBuffer() const { return _begin == buffer; }
    constexpr size_t theCapacity() const {
        return isUsingBuffer() ? SmallBufferSize : capacity;
    }

    constexpr byte* begin() { return _begin; }
    constexpr byte* end() { return _begin + size; }

    constexpr const byte* begin() const { return _begin; }
    constexpr const byte* end() const { return _begin + size; }

    void resize(size_t size);

    void setCapacity(size_t size);

    //removes the unused bytes
    constexpr void clearZeroes();
    //sets all unused bytes to 0
    constexpr void clearMemory();

    constexpr BigInt(size_t size, Sign sign);

    BigInt(std::initializer_list<byte> bytes)
            : BigInt(bytes.size(), Sign::Positive) {
        size_t i = 0;
        for (auto b: bytes) _begin[i++] = b;
    }

public:
    constexpr BigInt();

    template<Integral T>
    constexpr BigInt(T val);

    BigInt(std::string_view str);

    BigInt(const BigInt& val);
    BigInt(BigInt&& val) noexcept;
    BigInt& operator=(const BigInt& val);
    BigInt& operator=(BigInt&& val) noexcept;
    template<Integral T>
    constexpr BigInt& operator=(T val);

    ~BigInt() noexcept;

    constexpr bool isZero() const { return sign == Sign::Zero; }
    constexpr operator bool() const { return isZero(); }

    template<Integral T>
    constexpr T toInt(const char* msg = "Number too big for int") const;
    constexpr int64_t toInt64(const char* msg = "Number too big for int64") const;

    template<Integral T>
    constexpr explicit operator T() const { return toInt<T>(); }

    void toHex(std::ostream& s) const;
    void toDec(std::ostream& s) const;

    std::string toHex() const;
    std::string toDec() const;

    constexpr const BigInt& operator+() const { return *this; }
    BigInt operator-() const;

    friend BigInt operator|(const BigInt& lhs, const BigInt& rhs);
    friend BigInt operator|(const BigInt& lhs, Integral auto rhs) { return lhs | BigInt(rhs); }
    friend BigInt operator|(Integral auto lhs, const BigInt& rhs) { return rhs | BigInt(lhs); }

    friend BigInt operator&(const BigInt& lhs, const BigInt& rhs);
    friend BigInt operator&(const BigInt& lhs, Integral auto rhs) { return lhs & BigInt(rhs); }
    friend BigInt operator&(Integral auto lhs, const BigInt& rhs) { return rhs & BigInt(lhs); }

    BigInt& operator|=(const BigInt& rhs);
    BigInt& operator&=(const BigInt& rhs);


    friend BigInt operator<<(const BigInt& lhs, const BigInt& rhs);
    friend BigInt operator<<(const BigInt& lhs, Integral auto rhs) { return lhs << BigInt(rhs); }
    friend BigInt operator<<(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) << rhs; }

    friend BigInt operator>>(const BigInt& lhs, const BigInt& rhs);
    friend BigInt operator>>(const BigInt& lhs, Integral auto rhs) { return lhs >> BigInt(rhs); }
    friend BigInt operator>>(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) >> rhs; }

    BigInt& operator<<=(const BigInt& rhs);
    BigInt& operator<<=(Integral auto rhs) { return *this <<= BigInt(rhs); }
    BigInt& operator>>=(const BigInt& rhs);
    BigInt& operator>>=(Integral auto rhs) { return *this >>= BigInt(rhs); }

    constexpr bool operator ==(const BigInt& rhs) const;
    constexpr bool operator !=(const BigInt& rhs) const;
    constexpr friend int operator<=>(const BigInt& lhs, const BigInt& rhs);

    constexpr friend bool operator ==(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) == rhs; }
    constexpr friend bool operator ==(const BigInt& lhs, Integral auto rhs) { return lhs == BigInt(rhs); }

    constexpr friend bool operator !=(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) != rhs; }
    constexpr friend bool operator !=(const BigInt& lhs, Integral auto rhs) { return lhs != BigInt(rhs); }

    constexpr friend auto operator <=>(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) <=> rhs; }
    constexpr friend auto operator <=>(const BigInt& lhs, Integral auto rhs) { return lhs <=> BigInt(rhs); }

    constexpr static int absCmp(const BigInt& lhs, const BigInt& rhs);
    BigInt operator+(const BigInt& rhs) const;
    BigInt operator-(const BigInt& rhs) const;

    friend BigInt operator+(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) + rhs; }
    friend BigInt operator+(const BigInt& lhs, Integral auto rhs) { return lhs + BigInt(rhs); }

    friend BigInt operator-(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) - rhs; }
    friend BigInt operator-(const BigInt& lhs, Integral auto rhs) { return lhs - BigInt(rhs); }

    BigInt& operator+=(const BigInt& rhs);
    BigInt& operator-=(const BigInt& rhs);


    BigInt operator/(const BigInt& rhs) const;
    BigInt operator%(const BigInt& rhs) const;

    friend BigInt operator/(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) / rhs; }
    friend BigInt operator/(const BigInt& lhs, Integral auto rhs) { return lhs / BigInt(rhs); }

    friend BigInt operator%(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) % rhs; }
    friend BigInt operator%(const BigInt& lhs, Integral auto rhs) { return lhs % BigInt(rhs); }

    BigInt& operator/=(const BigInt& rhs);
    BigInt& operator%=(const BigInt& rhs);


    BigInt operator*(const BigInt& rhs) const;
    BigInt& operator*=(const BigInt& rhs);

    friend BigInt operator*(Integral auto lhs, const BigInt& rhs) { return BigInt(lhs) * rhs; }
    friend BigInt operator*(const BigInt& lhs, Integral auto rhs) { return lhs * BigInt(rhs); }

    static void divRem(const BigInt& a, const BigInt& b,
                       BigInt& quotient, BigInt& remainder);

private:
    enum class Op { Add, Sub };
    static BigInt& add(Op op, const BigInt& a, const BigInt& b, BigInt& res);
    static BigInt& mul(const BigInt& a, const BigInt& b, BigInt& res);
    static BigInt& mulImpl(const BigInt& a, const BigInt& b, BigInt& res);
    static BigInt& smallMulImpl(const BigInt& a, byte b, BigInt& res);

    void debugPrint(std::string_view msg, std::ostream& s) const;
    void debugPrint(std::string_view msg) const {
        debugPrint(msg, std::cout);
    }

    template<Op Op>
    friend BigInt& addImpl(const BigInt& a, const BigInt& b, BigInt& res) {
        return addImpl<Op>(a, b, res, a.size, b.size);
    }
    template<Op Op>
    friend BigInt& addImpl(const BigInt& a, const BigInt& b, BigInt& res,
                           size_t aSize, size_t bSize);

    void lshEqualsImpl(size_t rhs, size_t size);

    void splitNum(BigInt& low, BigInt& high, size_t where) const;

public:
    friend std::ostream& operator<<(std::ostream& stream, const BigInt& int_);
    friend std::istream& operator>>(std::istream& stream, BigInt& int_);
    friend class BigIntTester;
};


constexpr void BigInt::clearMemory() {
    for (size_t i = size, n = theCapacity(); i < n; ++i)
        _begin[i] = 0;
}
constexpr BigInt::BigInt() : BigInt(0) {}

constexpr BigInt::BigInt(size_t size, Sign sign) : BigInt() {
    resize(size);
    this->sign = sign;
}
template<Integral T>
constexpr BigInt::BigInt(T val)
        : _begin{buffer}, size(0), sign(Sign::Zero) {
    if (val == 0) {
        sign = Sign::Zero;
        clearMemory();
        return;
    }
    if (val < 0) {
        sign = Sign::Negative;
        val = -val;
    }
    else {
        sign = Sign::Positive;
    }
    for (;;) {
        _begin[size++] = (byte) val;
        val >>= 8;
        if (!val) break;
    }
    clearMemory();
}
template<Integral T>
constexpr BigInt& BigInt::operator=(T val) {
    size = 0;
    if (val == 0) {
        sign = Sign::Zero;
        return *this;
    }
    if (val < 0) {
        sign = Sign::Negative;
        val = -val;
    }
    else {
        sign = Sign::Positive;
    }
    for (;;) {
        _begin[size++] = val & 0xFF;
        val >>= 8;
        if (!val) break;

    }
    clearMemory();
    clearZeroes();
    return *this;
}

template<Integral T>
constexpr T BigInt::toInt(const char* msg) const {
    if (sign == Sign::Zero) return 0;
    if (size > sizeof(T)) {
        throw Error(msg);
    }
    if (!std::is_signed_v<T> && sign == Sign::Negative)
        throw Error("Can't cast a negative BigNum to unsigned");
    T res = 0;
    int i = 0;
    for (auto& b : *this) {
        res |= (b << i);
        i += 8;
    }
    return (std::is_signed_v<T> && sign == Sign::Negative) ? -res : res;
}
constexpr int64_t BigInt::toInt64(const char* msg) const {
    return toInt<int64_t>(msg);
}

constexpr void BigInt::clearZeroes() {
    int i = size;
    for (; i > 0; --i) {
        if (_begin[i-1]) break;
    }
    size = i;
    if (i == 0) sign = Sign::Zero;
}
constexpr bool BigInt::operator ==(const BigInt& rhs) const {
    if (sign != rhs.sign) return false;
    if (sign == Sign::Zero) return true;
    if (size != rhs.size) return false;

    for (size_t i = 0; i < size; ++i) {
        if (_begin[i] != rhs._begin[i]) return false;
    }
    return true;
}
constexpr bool BigInt::operator!=(const BigInt& rhs) const {
    return !(*this == rhs);
}

constexpr int BigInt::absCmp(const BigInt& lhs, const BigInt& rhs) {
    if (lhs.size > rhs.size) return 1;
    if (lhs.size < rhs.size) return -1;

    for (ssize_t i = lhs.size-1; i >= 0; --i) {
        if (lhs._begin[i] < rhs._begin[i]) {
            return -1;
        }
        if (lhs._begin[i] > rhs._begin[i]) {
            return 1;
        }
    }
    return 0;
}
constexpr int operator<=>(const BigInt& lhs, const BigInt& rhs) {
    // might return -2 or 2, but we care about the sign anyway
    if (lhs.sign != rhs.sign) return (int) lhs.sign - (int) rhs.sign;

    if (lhs.isZero()) return 0;
    return BigInt::absCmp(lhs, rhs) * (int) lhs.sign;
}
