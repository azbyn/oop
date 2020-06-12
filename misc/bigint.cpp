#include "bigint.h"

#include <sstream> // stringstream
#include <cstring> // memcpy

#include <iomanip>
#include <utility>
#include <array>

using byte = BigInt::byte;

void BigInt::setCapacity(size_t val) {
    // std::cout << "SET CAPACITY \n";
    _begin = new byte[val];
    capacity = val;
}
void BigInt::resize(size_t size, SL from) {
    // std::cout << "reSIZE: " << size << "\n";

    // size_t theCapacity = isUsingBuffer()? SmallBufferSize : capacity;
    if (theCapacity() >= size) {
        // std::cout << "============THE QUICK WAY " << theCapacity << "\n";
        this->size = size;
        return;
    }

    // std::cerr << from.line() << ":" << "resize: from "
    //           << this->size << " to " << size << "\n";
    // std::cerr << "CAPACITY" << capacity << "\n";
    // std::cerr << "=======\nzis " << *this << "\n========\n";
    byte* tmp = new byte[size];
    //const byte* tmp = _begin;
    //setCapacity(size);
    size_t i = 0;
    for (; i< this->size; ++i)
        tmp[i] = _begin[i];
    for (; i< size; ++i)
        tmp[i] = 0;
    // std::memcpy(tmp, _begin, this->size);
    // std::memset(_begin+this->size, 0, size - this->size);

    if (!isUsingBuffer()) {
        // std::cout << "CHANGED USING BUFFER resize "
        //           << (void*) _begin << "\n";
        // isUsingBuffer = false;
    // }
    // else {
        // std::cout << "deleet " << (void*) _begin << "\n";
        delete[] _begin;
    }
    this->size = size;
    this->capacity = size;
    this->_begin = tmp;
    // clearMemory();
    // std::cout << "tmp: "<< (void*) _begin << "\n";
    // std::cout << "tmp: "<< *this << "\n";

}

BigInt::~BigInt() noexcept {
    if (!isUsingBuffer()) {
        // std::cout << "delete: "<< (void*) _begin << "\n";
        delete[] _begin;
    }
}
BigInt::BigInt(const BigInt& val)
        : _begin(buffer), size(val.size), buffer {},
          // isUsingBuffer(true),
          sign(val.sign) {
    // std::cout<< "BI(&)\n";
    if (val.size > SmallBufferSize) {
        setCapacity(val.size);
        // isUsingBuffer = false;
        // std::cout << "CHANGED USING BUFFER BI(&)\n";
    }
    std::memcpy(_begin, val._begin, val.size);
    clearMemory();
}
BigInt::BigInt(BigInt&& val) noexcept
        : size(val.size),
          // isUsingBuffer(val.isUsingBuffer),
          sign(val.sign) {
    // std::cout<< "BI(&&)\n";
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

//if we already have isUsingBuffer we don't go back to the buffer
BigInt& BigInt::operator=(const BigInt& val) {
    // std::cout << "OP =(&) \n";
    // std::cout << "t:" <<*this<<"\n";
    // std::cout << "v:" <<val << "\n";

    this->size = val.size;
    this->sign = val.sign;
    if (!isUsingBuffer()) {
        if (capacity < val.size) {
            // std::cout << "OP =(&) deleet\n";
            delete[] _begin;
            setCapacity(val.size);
        }
    }
    else if (val.size > SmallBufferSize) {
        setCapacity(val.size);
        // std::cout << "CHANGED USING BUFFER BI=(&)\n"; // 
        // isUsingBuffer = false;
    }
    std::memcpy(_begin, val._begin, val.size);
    clearMemory();
    return *this;
}

BigInt& BigInt::operator=(BigInt&& val) noexcept {
    // std::cout << "OP =(&&) \n";
    this->~BigInt();

    if (val.isUsingBuffer()) {
        if (!isUsingBuffer()) {
            // std::cout << "OP =(&&) deleet\n";
            // delete[] _begin;
            _begin = buffer;
            // isUsingBuffer = true;
        }
        std::memcpy(_begin, val._begin, val.size);
    }
    else {
        capacity = val.capacity;
        _begin = std::exchange(val._begin, nullptr);
        size = val.size;
        // isUsingBuffer = std::exchange(val.isUsingBuffer, true);
    }
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
void BigInt::toDec(std::ostream& s) const { //TEST THIS
    switch (sign) {
    case Sign::Zero:
        s << "0";
        return;
    case Sign::Negative: s << "-"; break;
    case Sign::Positive: break;
    }
    BigInt a = *this;
    //we can have at most 3 digits per byte
    //std::vector<char> ();
    char* c = new char[a.size * 3];
    //ManagedArray<char> c(a.data.size() * 3);
    int i = 0;
    BigInt r;
    while (!a.isZero()) {
        divRem(a, 10, r, a);
        c[i++] = '0' + r._begin[0];
    }

    do {
        s << c[--i];
    }
    while (i != 0);
    delete[] c;
}

BigInt& BigInt::operator|=(const BigInt& rhs) {
    if (sign == Sign::Negative || rhs.sign == Sign::Negative)
        throw std::logic_error("Can't bitwise OR negative numbers");
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

    // if (sign == Sign::Negative || rhs.sign == Sign::Negative)
    //     throw std::logic_error("Can't bitwise AND negative numbers");

    size_t min = std::min(size, rhs.size);
    for (size_t i = 0; i < min; ++i) {
        _begin[i] &= rhs._begin[i];
    }
    clearZeroes();
    return *this;
}
BigInt operator&(const BigInt& lhs, const BigInt& rhs) {
    if (rhs.isZero() || lhs.isZero()) return 0;
    // if (lhs.sign == Sign::Negative || rhs.sign == Sign::Negative)
    //     throw std::logic_error("Can't bitwise AND negative numbers");

    size_t min = std::min(rhs.size, rhs.size);
    BigInt res(min);

    for (size_t i = 0; i < min; ++i) {
        res._begin[i] = lhs._begin[i] & rhs._begin[i];
    }

    res.clearZeroes();
    return res;
}

BigInt& BigInt::operator<<=(const BigInt& _rhs) {
    auto rhs = _rhs.toInt64("Can't shift left by that much");

    if (rhs < 0) return *this >>= -rhs;

    // std::cout << *this << "<<=" << rhs << "\n";
    uint64_t bigShift = rhs / 8;
    uint64_t smallShift = rhs % 8;

    resize(size + bigShift + (smallShift != 0) );

    uint16_t carry = 0;
    for (auto& b : *this) {
        carry = (b << smallShift) | carry;
        b = carry;
        carry >>= 8;
    }
    //big Shift
    auto mid = _begin + bigShift;
    std::copy(_begin, end(), mid);
    std::fill(_begin, mid, 0);
    // std::cout << "res:" << *this << "\n";
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
    auto rhs = _rhs.toInt64("Can't shift left by that much");

    if (rhs < 0) return *this <<= -rhs;
    uint64_t bigShift = rhs / 8;
    if (bigShift >= size) return *this = 0;
    resize(size - bigShift);

    uint64_t smallShift = rhs % 8;
    uint16_t carry = 0;
    _begin[0] >>= smallShift;
    for (size_t i = 1; i < size; ++i) {
        carry = (_begin[i] << (8 - smallShift));
        _begin[i] = carry >> 8;
        _begin[i-1] |= carry & 0xFF;
    }
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
    // std::cout << "zis: "<< *this << "\n";
    return *this;
}
BigInt& BigInt::operator%=(const BigInt& rhs) {
    BigInt q, r;
    divRem(*this, rhs, q, r);
    // std::cout << "R: " << r;
    return *this = r;

    // BigInt q;
    // divRem(*this, rhs, q, *this);
    // return *this;
}

//test combinations of { } from an array
// where res could be either a or b
// static bool debug = false;
BigInt& BigInt::add(Op op, const BigInt& a, const BigInt& b, BigInt& res) {
    // ::debug = a == 0x323232 && b == 2;
    //the mismatch of >= and > is important so that zero works properly
    bool bSign = !((op == Op::Add) ^ (b.sign >= 0));
    bool aSign = a.sign >= 0;

    // std::cout  << "+: " << (a).toInt64() <<  "," << (int) b.toInt64() << "\n";
    // std::cout  << "sign: " << (int) b.sign <<  "," << (int) a.sign << "\n";
    // std::cout  << "s-gn: " << (int) bSign <<  "," << (op == Op::Add) << "\n";

    if (a.sign == Sign::Zero) {
        res = b;
        if (op != Op::Add) res.sign = -b.sign;
        //else res.sign = -b.sign;
        // return addImpl<Op::Add>(a, b, res);
        return res;
    }

    if (b.sign == Sign::Zero) {
        res = a;
        return res;
    }

    if (aSign == bSign) {
        // std::cout << "GOES TO IMPL\n";
        res.sign = a.sign;
        // std::cout << "!sizes: " << a.size << ", "<< b.size << "\n";
        // std::cout << "!signs: " << (int)a.sign << ", "<< (int)b.sign
        //           << "; " << (int) res.sign << "\n";
        if (a.size > b.size){
            return addImpl<Op::Add>(a, b, res);
        }
        return addImpl<Op::Add>(b, a, res);
    }
    int cmp = absCmp(a, b);
    // std::cout << "a: "<< a << "\n";
    // std::cout << "b: "<< b << "\n";
    // std::cout << "cmp: "<< std::dec<< cmp << "\n";
    if (cmp == 0) {
        return res = 0;
    }
    bool delta = cmp >= 0; // >= from the table

    bool inverted = aSign ^ bSign ^ delta; // do b - a
    bool minus = bSign == delta; // do -(whatever)
    res.sign = minus ? Sign::Negative : Sign::Positive;
    if (inverted) {
        // std::cout << "invert\n";
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
BigInt& addImpl(const BigInt& a, const BigInt& b, BigInt& res) {
    // a._begin might be == res._begin or b._begin == res._begin
    size_t aSize = a.size;
    size_t bSize = b.size;
    // we know that a.size >= b.size
    // we know we can't overflow by subtracting
    // just to be safe
    // redundant zeroes will be removed
    if constexpr (Op == BigInt::Op::Add)
        res.resize(a.size + 1);
    else res.resize(a.size);
    // constexpr bool debug = true;
    constexpr bool debug = false;

    // if (debug) {
        // std::cout << "a: "<< a << "\n";
        // std::cout << "b: "<< b << "\n";
        // std::cout << "r: "<< res << "\n";
    // }

    // std::cout << "==========: \n";
    if (debug)
        std::cout << "sizes: " << aSize << ", "<< bSize << "\n";
    uint16_t carry = 0;
    size_t i = 0;
    if constexpr (Op == BigInt::Op::Add) {
        // std::cout << "+0?\n";
        for (; i < bSize; ++i) {
            carry += a._begin[i] + b._begin[i];
            res._begin[i] = (byte)carry;
            carry >>= 8;
        }
        // std::cout << "i:" << i << "\n";
        // std::cout << "carry:" << carry << "\n";
        // std::cout << "oi++" << res << "\n";

        if (aSize > i) {
            if (carry) {
                if (debug) std::cout << "OIDA\n";
                for (;;) {
                    //we could do it without the check, but let's be safe
                    if (i >= aSize) goto koniec;
                    res._begin[i] = a._begin[i]+ carry;
                    auto rbyte = res._begin[i];
                    // auto abyte = a._begin[i];
                    // std::cout <<"z: " << i << ":"
                    //           << std::hex << (int) rbyte << "-"
                    //           << std::hex << (int) abyte << "\n";
                    ++i;
                    // if (abyte != 0xFF) break;
                    if (rbyte != 0) break;
                }
                // while (i < aSize && a._begin[i] == 0) {
                //     std::cout << "POI\n";
                //     res._begin[i++] = 0xFF;
                // }
            
                // res._begin[i] = a._begin[i] - carry;
                // ++i;
            } else {
                
                // std::cout << "NOID\n";
            }
            // std::cout << "ahas:" << std::hex << (int)a._begin[i] << "\n";
            // carry += a._begin[i];
            // res._begin[i] = (byte) carry;// + a._begin[i];
            // // std::cout << "hex:" << std::hex << carry << "\n";
            // // if (carry & 0xFF00)
            // res._begin[i+1] = (carry >> 8) + a._begin[i+1];
            // ++i;
            // std::cout << "oi" << (int) res._begin[i] << "\n";
        } else {
            // std::cout << "OI ZIS IS IT\n";
            res._begin[i] = carry;
            ++i;
        }
        // std::cout << "oi" << (int) res._begin[i] << "\n";
        // std::cout << "oi--" << res << "\n";

        // std::cout << "r: "<< res << "\n";
        for (; i < aSize; ++i) {
            // std::cout << "set more bits\n";
            // std::cout << "a:" << (int) a._begin[i] << "\n";
            res._begin[i] = a._begin[i];
        }
        // std::cout << "r: "<< res << "\n";
    }
    else {
        // std::cout  << "MINUS\n";
        for (; i < bSize; ++i) {
            int16_t r = a._begin[i] - b._begin[i] - carry;
            // carry = a._begin[i] - b._begin[i] - carry;
            // std::cout << "R: " << (int)a._begin[i]
            //           << " - " << (int)b._begin[i] << ";" << carry << "\n";
            if (r < 0) {
                carry = 1;
                // std::cout << "B: " << r << "\n";
                r = 0x0100 + r;
                // std::cout << "negative: " << r << "\n";
            } else carry = 0;
            // std::cout << "c: " << std::hex << i << " " << (int) carry << "\n";
            // std::cout << "r: " << (int) a._begin[i] << "-"
            //           << (int) b._begin[i] << "\n";
            // std::cout << "res_: " << i << " " << (int) r << "\n";
            res._begin[i] = (byte) r;// carry;
            // std::cout << "<: "<< res << "\n";
        }

        // std::cout << "HI: "<< res << "\n";
        // std::cout << "oi" << (int) res._begin[i] << "\n";
        // if (aSize > i) {
        //     // if (a._begin[i] < carry) {
        //     //     res._begin[i+1] = a._begin[i] - carry;
        //     // }
        //     std::cout << "big" << (int) res._begin[i] << "\n";
        //     res._begin[i] = a._begin[i] - carry;
        // }
        // std::cout << "carry:" << carry << "\n";
        if (carry) {
            // std::cout << 
            for (;;) {
                //we could do it without the check, but let's be safe
                if (i >= aSize) goto koniec;
                auto abyte = a._begin[i];
                res._begin[i] = a._begin[i] - carry;
                // std::cout <<"ż: " << i << ":" << std::hex << (int) abyte << "\n";
                ++i;
                if (abyte != 0) break;
            }
            // while (i < aSize && a._begin[i] == 0) {
            //     std::cout << "POI\n";
            //     res._begin[i++] = 0xFF;
            // }
            
            // res._begin[i] = a._begin[i] - carry;
            // ++i;
        }
        // else {
        for (; i < aSize; ++i) {
            // if (a._begin[i] < carry) {
            //     res._begin[i+1] = a._begin[i] - carry;
            // }
            // std::cout << "set more bits\n";
            // std::cout << "a:" << (int) a._begin[i] << "\n";
            res._begin[i] = a._begin[i];
        }
        // }
        // ++i;
        // for (; i < aSize; ++i) {
        //     // std::cout << "a:" << (int) a._begin[i] << "\n";
        //     res._begin[i] = 0;
        // }
        // std::cout << "END: "<< res << "\n"; //
    }
    // std::cout << "res: "<< res << "\n";
koniec:
    res.clearZeroes();
    return res;
}


//https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
//for simplicity we use the form:
//a = q * b + r
//do smth like phony_q
void BigInt::divRem(const BigInt& a, const BigInt& b,
                    BigInt& q, BigInt& r) {
    if (b == 0) throw std::logic_error("Division by 0");
    // std::cout << "!sizes: " << a.size << ", "<< b.size << "\n";
    //a fairly dumb test, but we do it just in case
    if (b._begin == r._begin || q._begin == b._begin)
        throw std::logic_error("divRem: b, q and r must be references to"
                               " differnt objects");

    if (absCmp(b, a) > 0) {
        // std::cout << "EARLY QUIT\n";
    // if (b > a) {
        //we don't do q = 0 first because it might be the same data as a
        r = a;
        q = 0;
        return;
    }
    size_t bSize = b.size;
    size_t aSize = a.size;
    // we have r < b, so we alocate memory for the worst case
    r.resize(bSize);
    for (auto& bb: r) bb = 0;
    //r = 0;
    // std::cout << "r+:" << r << "\n";
    // std::cout << "q+:" << r << "\n";
    //if (a._begin != q._begin) {
    q.resize(aSize-bSize + 1);//log(a/b) = log(a) - log(b)
    //}
    // std::cout << "!p2: " << r.size << ", "<< q.size << "\n";
    // std::cout << "q=:" << r << "\n";
    // for (ssize_t i = a.size-1; i >= 0; ++i) {
    for (ssize_t i = aSize -1; i >= 0; --i) {
        auto *ptr = q._begin + i;
        byte abyte = a._begin[i];
        *ptr = 0;
        for (int bitIndex = 7; bitIndex >= 0; --bitIndex) {
            // std::cout << "BEGIN <<=\n";
            r <<= 1;
            // std::cout << "END <<=\n";
            // get bit
            // std::cout << "before|=\n";
            // r |= (abyte >> bitIndex) & 1;
            r._begin[0] |= (abyte >> bitIndex) & 1;
            // std::cout << "after|=\n";
            // std::cout << std::hex << ":" << r.capacity <<"\n";
            // r |= (*ptr >> bitIndex) & 1;
            // if (i == aSize-1) std::cout << "r: " << r << "\n";
            // std::cout << "q<:" << q << "\n";
            // we can't do r -= b cuz we don't care about the sign
            // nor can we do r >= b
            if (absCmp(r, b) >= 0) {
                //r -= b;//.toInt64();
                // std::cout << "r:" << r.toInt64() << " " << b.toInt64() << "\n";
                addImpl<Op::Sub>(r, b, r);
                // std::cout << std::hex << "--" << r.capacity <<"\n";

                // set bit
                *ptr |= (1 << bitIndex);
                // std::cout << "SET\n";
                // std::cout << "BIG " << q << "\n";
            }
            // else {
            //     // unset bit
            //     //std::cout << "UNSET\n";
            //     *ptr &= ~(1 << bitIndex);
            // }
            // std::cout << "q>:" << q << "\n";
        }
    }
    // std::cout << "q-:" << q << "\n";
    r.sign = a.sign;// Sign::Positive;// b.sign;
    q.sign = a.sign * b.sign;
    // std::cout << "r-:" << r << "\n";
    r.clearZeroes();
    q.clearZeroes();
    // std::cout << "end_div\n";
}

std::ostream& operator<<(std::ostream& s, const BigInt& bi) {
    // int_.toDec(stream);
    s << "[";
    bi.toHex(s);
    s << "\n<usingBuffer: " << bi.isUsingBuffer() << ">\n";
    s << "<size: " << std::dec << bi.size << ">\n";
    s << "<capacity: " << bi.capacity << ">\n";
    s << "<sign: " << (int) bi.sign << ">\n";
    s << "<_begin: " << (void*) bi._begin << ">\n";
    s << "<buf: ";
    for (size_t i = 0; i < bi.size; ++i) {
        s << std::hex << std::setw(2) << (int)bi._begin[i]<<"-";
    }
    s << ">]\n" << std::dec;
    return s;
}
