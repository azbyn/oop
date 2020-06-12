#include "bigint.h"

#include "misc.h"

#include <sstream> // std::stringstream
#include <cstring> // std::memcpy

#include <iomanip> // std::dec, std::hex
#include <utility> // std::move
#include <array>   // std::array



using byte = BigInt::byte;

void BigInt::setCapacity(size_t val) {
    _begin = new byte[val];
    capacity = val;
}
void BigInt::resize(size_t size) {
    if (theCapacity() >= size) {
        this->size = size;
        return;
    }

    byte* tmp = new byte[size];
    size_t i = 0;
    for (; i< this->size; ++i)
        tmp[i] = _begin[i];
    for (; i< size; ++i)
        tmp[i] = 0;

    if (!isUsingBuffer()) {
        delete[] _begin;
    }
    this->size = size;
    this->capacity = size;
    this->_begin = tmp;
}

BigInt::~BigInt() noexcept {
    if (!isUsingBuffer()) {
        delete[] _begin;
    }
}
BigInt::BigInt(const BigInt& val)
        : _begin(buffer), size(val.size), buffer {},
          sign(val.sign) {
    if (val.size > SmallBufferSize) {
        setCapacity(val.size);
    }
    std::memcpy(_begin, val._begin, val.size);
    clearMemory();
}
BigInt::BigInt(BigInt&& val) noexcept
        : size(val.size),
          sign(val.sign) {
    if (val.isUsingBuffer()) {
        _begin = buffer;
        std::memcpy(_begin, val._begin, val.size);
    }
    else {
        capacity = val.capacity;
        _begin = std::exchange(val._begin, nullptr);
    }
    clearMemory();
}

BigInt::BigInt(std::string_view sv) : BigInt() {
    // improveable
    std::string str = std::string(sv);
    std::stringstream ss(str);
    ss >> *this;
}

//if we already have isUsingBuffer we don't go back to the buffer
BigInt& BigInt::operator=(const BigInt& val) {
    if (_begin == val._begin) return *this;

    this->size = val.size;
    this->sign = val.sign;
    if (!isUsingBuffer()) {
        if (capacity < val.size) {
            delete[] _begin;
            setCapacity(val.size);
        }
    }
    else if (val.size > SmallBufferSize) {
        setCapacity(val.size);
    }
    std::memcpy(_begin, val._begin, val.size);
    clearMemory();
    return *this;
}

BigInt& BigInt::operator=(BigInt&& val) noexcept {
    if (_begin == val._begin) {
        std::cerr << "=====TY DEBIL\n";
        exit(-1);
    }

    this->~BigInt();

    if (val.isUsingBuffer()) {
        if (!isUsingBuffer()) {
            _begin = buffer;
        }
        std::memcpy(_begin, val._begin, val.size);
    }
    else {
        capacity = val.capacity;
        _begin = val._begin;
        val._begin = nullptr;
    }
    size = val.size;
    sign = val.sign;

    clearMemory();

    return *this;
}

static constexpr char hexChar(int i) {
    if (i < 10) return '0' + i;
    return 'A' - 10 + i;
}

std::string BigInt::toHex() const {
    std::stringstream ss;
    toHex(ss);
    return ss.str();
}

std::string BigInt::toDec() const {
    std::stringstream ss;
    toDec(ss);
    return ss.str();
}
void BigInt::toHex(std::ostream& s) const {
    switch (sign) {
    case Sign::Zero:
        s << "0";
        return;
    case Sign::Negative: s << "-"; break;
    case Sign::Positive: s << "+"; break;
    }
    for (int i = size-1; i >= 0; --i) {
        s << hexChar(_begin[i] >> 4);
        s << hexChar(_begin[i] & 0xF);
    }
}
void BigInt::toDec(std::ostream& s) const {
    switch (sign) {
    case Sign::Zero:
        s << "0";
        return;
    case Sign::Negative: s << "-"; break;
    case Sign::Positive: break;
    }
    BigInt a = *this;
    //we can have at most 3 digits per byte
    char* c = new char[a.size * 3];

    int i = 0;
    BigInt r;
    while (!a.isZero()) {
        divRem(a, 10, a, r);
        c[i++] = '0' + r._begin[0];
    }

    do {
        s << c[--i];
    } while (i != 0);

    delete[] c;
}

BigInt& BigInt::operator|=(const BigInt& rhs) {
    if (sign == Sign::Negative || rhs.sign == Sign::Negative)
        throw Error("Can't bitwise OR negative numbers");
    if (rhs.isZero()) return *this;
    if (isZero()) return *this = rhs;

    if (this->size < rhs.size) {
        resize(rhs.size);
    }
    for (size_t i = 0; i < rhs.size; ++i) {
        _begin[i] |= rhs._begin[i];
    }
    clearZeroes();
    return *this;
}

BigInt operator|(const BigInt& lhs, const BigInt& rhs) {
    if (lhs.size < rhs.size) {
        BigInt res = rhs;
        res |= lhs;
        return res;
    }
    BigInt res = lhs;
    res |= rhs;
    return res;
}

BigInt& BigInt::operator&=(const BigInt& rhs) {
    if (rhs.isZero()) return *this = 0;
    if (isZero()) return *this;

    size_t min = std::min(size, rhs.size);
    for (size_t i = 0; i < min; ++i) {
        _begin[i] &= rhs._begin[i];
    }
    size = min;
    clearZeroes();
    return *this;
}
BigInt operator&(const BigInt& lhs, const BigInt& rhs) {
    if (rhs.isZero() || lhs.isZero()) return 0;

    size_t min = std::min(lhs.size, rhs.size);
    BigInt res(min);

    for (size_t i = 0; i < min; ++i) {
        res._begin[i] = lhs._begin[i] & rhs._begin[i];
    }
    res.clearZeroes();
    return res;
}

void BigInt::lshEqualsImpl(size_t rhs, size_t size) {
    uint64_t bigShift = rhs / 8;
    uint64_t smallShift = rhs % 8;


    resize(size + bigShift + (smallShift != 0));

    uint16_t carry = 0;
    if (smallShift) {
        for (auto& b : *this) {
            carry = (b << smallShift) | carry;
            b = (byte) carry;
            carry >>= 8;
        }
    }
    ssize_t i = this->size - 1;
    for (; i >= (ssize_t) bigShift; --i) {
        _begin[i] = _begin[i-bigShift];
    }
    for (; i >= 0; --i)
        _begin[i] = 0;
}

BigInt& BigInt::operator<<=(const BigInt& _rhs) {
    auto rhs = _rhs.toInt64("Can't shift left by that much");

    if (rhs < 0) throw Error("Negative shift");
    lshEqualsImpl(rhs, size);
    clearZeroes();
    return *this;
}
BigInt operator<<(const BigInt& lhs, const BigInt& rhs) {
    // improveable (custom implementation)
    BigInt res = lhs;
    res <<= rhs;
    return res;
}

BigInt& BigInt::operator>>=(const BigInt& _rhs) {
    if (_rhs.size > sizeof(int64_t)) return *this = 0;
    auto rhs = _rhs.toInt64();

    if (rhs < 0) Error("Negative shift");
    uint64_t bigShift = rhs / 8;
    if (bigShift >= size) return *this = 0;

    uint64_t smallShift = rhs % 8;
    size_t newSize = size - bigShift;
    uint16_t carry = 0;

    _begin[0] = _begin[bigShift] >> smallShift;

    for (size_t i = 1; i < newSize; ++i) {
        carry = (_begin[i+bigShift] << (8 - smallShift));
        _begin[i] = carry >> 8;
        _begin[i-1] |= carry;
    }
    resize(newSize);
    clearZeroes();
    return *this;
}
BigInt operator>>(const BigInt& lhs, const BigInt& rhs) {
    // improveable (custom implementation)
    BigInt res = lhs;
    res >>= rhs;
    return res;
}
BigInt BigInt::operator-() const {
    BigInt res = *this;
    res.sign = -res.sign;
    return res;
}

BigInt BigInt::operator+(const BigInt& rhs) const {
    BigInt res;
    add(Op::Add, *this, rhs, res);
    return res;
}

BigInt BigInt::operator-(const BigInt& rhs) const {
    BigInt res;
    add(Op::Sub, *this, rhs, res);
    return res;
}

BigInt& BigInt::operator+=(const BigInt& rhs) {
    return add(Op::Add, *this, rhs, *this);
}
BigInt& BigInt::operator-=(const BigInt& rhs) {
    return add(Op::Sub, *this, rhs, *this);
}

BigInt BigInt::operator/(const BigInt& rhs) const {
    BigInt q, r;
    divRem(*this, rhs, q, r);
    return q;
}

BigInt BigInt::operator%(const BigInt& rhs) const {
    BigInt q, r;
    divRem(*this, rhs, q, r);
    return r;
}

BigInt& BigInt::operator/=(const BigInt& rhs) {
    BigInt r;
    divRem(*this, rhs, *this, r);
    return *this;
}
BigInt& BigInt::operator%=(const BigInt& rhs) {
    BigInt q, r;
    divRem(*this, rhs, q, r);
    return *this = r;
}

BigInt BigInt::operator*(const BigInt& rhs) const {
    BigInt res;
    mul(*this, rhs, res);
    return res;
}
BigInt& BigInt::operator*=(const BigInt& rhs) {
    BigInt res;
    mul(*this, rhs, *this);
    return *this;
}

// where res could be either a or b
BigInt& BigInt::add(Op op, const BigInt& a, const BigInt& b, BigInt& res) {
    bool bSign = !((op == Op::Add) ^ (b.sign >= 0));
    bool aSign = a.sign >= 0;

    if (a.sign == Sign::Zero) {
        res = b;
        if (op != Op::Add) res.sign = -b.sign;
        return res;
    }

    if (b.sign == Sign::Zero) {
        res = a;
        return res;
    }

    if (aSign == bSign) {
        res.sign = a.sign;
        if (a.size > b.size) {
            return addImpl<Op::Add>(a, b, res);
        }
        return addImpl<Op::Add>(b, a, res);
    }

    //we must be subtracting if we got here
    int cmp = absCmp(a, b);

    if (cmp == 0) {
        return res = 0;
    }
    bool delta = cmp >= 0; // >= from the table

    bool inverted = aSign ^ bSign ^ delta; // do b - a
    bool minus = bSign == delta; // do -(whatever)
    res.sign = minus ? Sign::Negative : Sign::Positive;
    if (inverted) {
        return addImpl<Op::Sub>(b, a, res);
    }
    return addImpl<Op::Sub>(a, b, res);
    /*

      b = b.sign * (isNegative? -1: 1)
      a = a.sign
      | a | b | a>0 | b>0 | >= | minus | inverted | op     | preview |
      |---+---+-----+-----+----+-------+----------+--------+---------|
      | + | + |   1 |   1 |  ? |     0 |        0 | a +b   | AA+BB   |
      | + | - |   1 |   0 |  1 |     0 |        0 | a-b    | AA-B    |
      | + | - |   1 |   0 |  0 |     1 |        1 | -(b-a) | A-BB    |
      | - | + |   0 |   1 |  1 |     1 |        0 | -(a-b) | -AA+B   |
      | - | + |   0 |   1 |  0 |     0 |        1 | b-a    | -A+BB   |
      | - | - |   0 |   0 |  ? |     1 |        0 | -(a+b) | -AA-BB  |

      inverted = a.s^b.s^(>=) # where ? = 0m
      minus = (a>0) == (>=)
      if (b.sign == b.sign) return (AA+BB) *sign
    */
}

template<BigInt::Op Op> // a and b are treated as positive, .sign doesn't matter
BigInt& addImpl(const BigInt& a, const BigInt& b, BigInt& res,
                size_t aSize, size_t bSize) {

    // redundant zeroes will be removed
    if constexpr (Op == BigInt::Op::Add)
        res.resize(aSize + 1);
    else res.resize(aSize);

    uint16_t carry = 0;
    size_t i = 0;
    if constexpr (Op == BigInt::Op::Add) {
        for (; i < bSize; ++i) {
            carry += a._begin[i] + b._begin[i];
            res._begin[i] = (byte)carry;
            carry >>= 8;
        }
        if (aSize > i) {
            if (carry) {
                for (;;) {
                    if (i >= aSize) {
                        res._begin[i] = carry;
                        goto end;
                    }
                    res._begin[i] = a._begin[i] + carry;
                    auto rbyte = res._begin[i];
                    ++i;
                    if (rbyte != 0)
                        break;
                }
            }
        } else {
            res._begin[i] = carry;
            ++i;
        }

        for (; i < aSize; ++i) {
            res._begin[i] = a._begin[i];
        }
    }
    else {
        for (; i < bSize; ++i) {
            int16_t r = a._begin[i] - b._begin[i] - carry;
            if (r < 0) {
                carry = 1;
                r = 0x0100 + r;
            }
            else carry = 0;
            res._begin[i] = (byte) r;
        }

        if (carry) {
            for (;;) {
                if (i >= aSize) goto end;
                auto abyte = a._begin[i];
                res._begin[i] = a._begin[i] - carry;

                ++i;
                if (abyte != 0) break;
            }
        }
        for (; i < aSize; ++i) {
            res._begin[i] = a._begin[i];
        }
    }

end:

    res.clearZeroes();
    return res;
}


//https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
//for simplicity we use the form:
//a = q * b + r
void BigInt::divRem(const BigInt& a, const BigInt& b,
                    BigInt& q, BigInt& r) {
    if (b == 0) throw Error("Division by 0");

    if (b._begin == r._begin || q._begin == b._begin)
        throw Error("divRem: b, q and r must be references to"
                               " different objects");

    if (absCmp(b, a) > 0) {
        //we don't do q = 0 first because it might be the same data as a
        r = a;
        q = 0;
        return;
    }
    size_t bSize = b.size;
    size_t aSize = a.size;

    // we have r < b, so we alocate memory for the worst case
    size_t rSize = bSize+1;
    size_t qSize = aSize;

    auto rSign = a.sign;
    auto qSign = a.sign * b.sign;

    r.resize(rSize);
    for (auto& bb: r) bb = 0;

    q.resize(qSize);

    auto absCmpRemainder = [&](const BigInt& r, const BigInt& b) {
        if (r._begin[bSize] != 0) {
            return 1;
        }
        for (ssize_t i = bSize-1; i >= 0; --i) {
            if (r._begin[i] < b._begin[i]) {
                return -1;
            }
            if (r._begin[i] > b._begin[i]) {
                return 1;
            }
        }
        return 0;
    };

    r.sign = rSign;
    q.sign = qSign;

    for (ssize_t i = aSize -1; i >= 0; --i) {
        auto *ptr = q._begin + i;
        byte abyte = a._begin[i];
        *ptr = 0;
        for (int bitIndex = 7; bitIndex >= 0; --bitIndex) {
            r.lshEqualsImpl(1, bSize);

            r._begin[0] |= (abyte >> bitIndex) & 1;
            if (absCmpRemainder(r, b) >= 0) {
                addImpl<Op::Sub>(r, b, r, rSize, bSize);
                // set bit
                *ptr |= (1 << bitIndex);
            }
        }
    }
    r.sign = rSign;
    q.sign = qSign;
    r.clearZeroes();
    q.clearZeroes();
}
BigInt& BigInt::mul(const BigInt& a, const BigInt& b, BigInt& res) {
    auto sign = a.sign * b.sign;
    mulImpl(a, b, res);
    res.sign = sign;
    return res;
}
//we know low != this and high != this
void BigInt::splitNum(BigInt& low, BigInt& high, size_t n) const {
    low.resize(n);
    high.resize(size - n);
    low.sign = high.sign = Sign::Positive;
    size_t i = 0;
    for (; i < n; ++i) {
        low._begin[i] = _begin[i];
    }

    for (; i < size; ++i) {
        high._begin[i-n] = _begin[i];
    }
}
// doesn't care about signs, assumes positive
BigInt& BigInt::smallMulImpl(const BigInt& a, byte b, BigInt& res) {
    auto aSize = a.size;
    // ceil(log_256(a*b)) = ceil(log_256(a)) + ceil(log_256(b)) <=
    //      <= ceil(log_256(a)) + ceil(log_256(255)) = a.size + 1
    res.resize(a.size + 1);
    int64_t carry = 0;

    for (size_t i = 0; i < aSize; ++i) {
        carry += a._begin[i] * b;
        res._begin[i] = (byte) carry;
        carry >>= 8;
    }
    res._begin[aSize] = carry;
    res.sign = Sign::Positive;

    res.clearMemory();
    res.clearZeroes();

    return res;
}


// doesn't care about signs
//https://en.wikipedia.org/wiki/Karatsuba_algorithm
BigInt& BigInt::mulImpl(const BigInt& a, const BigInt& b, BigInt& res) {
    if (a.size <= 1) {
        smallMulImpl(b, a._begin[0], res);
        return res;
    }
    if (b.size <= 1) {
        smallMulImpl(a, b._begin[0], res);
        return res;
    }
    size_t m = std::min(a.size, b.size);
    size_t m2 = m/2;

    BigInt lowA, highA, lowB, highB;

    a.splitNum(lowA, highA, m2);
    b.splitNum(lowB, highB, m2);

    BigInt& z2 = res;
    BigInt z0, z1;

    mulImpl(highA, highB, z2);
    mulImpl(lowA, lowB, z0);

    lowA += highA;
    lowB += highB;

    mulImpl(lowA, lowB, z1);

    z1 -= z2;
    z1 -= z0;

    z2 <<= m2*2*8;
    z1 <<= m2*8;

    z2 += z1;
    z2 += z0;

    return res;
}

void BigInt::debugPrint(std::string_view msg, std::ostream& s) const {
    auto& bi = *this;
    s << msg << "[";
    bi.toHex(s);
    // s << "\n<usingBuffer: " << bi.isUsingBuffer() << ">\n";
    s << "<size: " << std::dec << bi.size << ">";
    s << "<capacity: " << std::dec << bi.capacity << ">";
    s << "<sign: " << (int) bi.sign << ">";
    s << "<_begin: " << (void*) bi._begin << ">";
    s << "<buf: ";
    for (size_t i = 0; i < bi.theCapacity(); ++i) {
        s << std::hex << std::setw(2) << (int)bi._begin[i];
        if (i%2==1)
            s << "-";
    }
    s << "]" << std::dec << "\n";
}
std::ostream& operator<<(std::ostream& s, const BigInt& bi) {
    bi.toDec(s);
    return s;
}

std::istream& operator>>(std::istream& s, BigInt& res) {
    using Sign = BigInt::Sign;
    res = 0;
    auto c = s.get();
    Sign sign = Sign::Positive;

    while (!s.eof() && isspace(c)) c = s.get();

    switch (c) {
    case '-':
        sign = Sign::Negative;
        [[fallthrough]];
    case '+':
        c = s.get();
        break;
    }

    for (;;) {
        unsigned digit = c - '0';
        if (digit >= 10)
            throw BigInt::Error("Invalid BigInt");
        BigInt::smallMulImpl(res, 10, res);
        res += digit;
        c = s.get();

        if (c == std::char_traits<char>::eof() || isspace(c))
            break;
    }
    res.sign = sign;
    res.clearZeroes();

    return s;
}
