#include "bigint.h"

#include <sstream> // stringstream
#include <cstring> // memcpy

#include <iomanip>
#include <utility>
#include <array>


template<typename... Args>
inline std::string concat(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}

using byte = BigInt::byte;

void BigInt::setCapacity(size_t val) {
    // std::cout << "SET CAPACITY \n";
    _begin = new byte[val];
    capacity = val;
}
void BigInt::resize(size_t size) {

    // size_t theCapacity = isUsingBuffer()? SmallBufferSize : capacity;
    if (theCapacity() >= size) {
        // std::cout << "============THE QUICK WAY " << theCapacity << "\n";
        this->size = size;
        return;
    }
    if (Debug)
        std::cout << "reSIZE: " << size << "\n";

    // std::cerr << ":" << "resize: from "
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
    // std::cout << "tmp: "<< (void*) _begin << "\n";
    // debugPrint("tmp: ", std::cout);

}

BigInt::~BigInt() noexcept {
    if (!isUsingBuffer()) {
        // if (Debug)
        // std::cout << "delete '"<< "': " << (void*) _begin << "\n";
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



BigInt::BigInt(std::string_view sv) : BigInt() {
    // resize(sv.size()/2);
    // size = 0;
    // improveable (this is horrible)
    std::string str = std::string(sv);
    std::stringstream ss(str);
    ss >> *this;
}

//if we already have isUsingBuffer we don't go back to the buffer
BigInt& BigInt::operator=(const BigInt& val) {
    if (_begin == val._begin) return *this;
    // std::cout << "OP =(&) \n";
    // this->debugPrint("this:");
    // val.debugPrint("val :");
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
        // std::cout << "CHANGED USING BUFFER BI=(&)\n";
        // isUsingBuffer = false;
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
    // std::cout << "OP =(&&) \n";
    // if (Debug) {
    //     std::cout << "breaky breaki\n";
    //     this->debugPrint("==dis:");
    //     val.debugPrint("==val:");
    // }
    this->~BigInt();

    // std::cout << "sgn " << (int) sign << "\n";
    // sign = val.sign;
    // std::cout << "sgn " << (int) sign << "\n";

    if (val.isUsingBuffer()) {
        // std::cout << "BUFFY VAL\n";
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
        _begin = val._begin;
        val._begin = nullptr;
        // _begin = std::exchange(val._begin, nullptr);
        // isUsingBuffer = std::exchange(val.isUsingBuffer, true);
    }
    size = val.size;
    sign = val.sign;
    // std::cout << "sgn " << (int) sign << "\n";
    // this->debugPrint("mofoz:");
    clearMemory();// it overwrites the sign sometimes? 
    // this->debugPrint("mofzo:");
    // std::cout << "sgn " << (int) sign << "\n";
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
    //std::vector<char> ();
    char* c = new char[a.size * 3];
    //ManagedArray<char> c(a.data.size() * 3);
    int i = 0;
    BigInt r;
    while (!a.isZero()) {
        divRem(a, 10, a, r);
        // std::cout << "oi " << (int) r._begin[0] << "\n";
        // std::cout << "a " << a << "\n";
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

void BigInt::lshEqualsImpl(size_t rhs, size_t size) {
    // std::cout << "BIEFBBSHJFMD" << *this << "\n";
    uint64_t bigShift = rhs / 8;
    uint64_t smallShift = rhs % 8;


    // std::cout << "PRs: " << *this << " << " << rhs <<"\n";
    resize(size + bigShift + (smallShift != 0));
    // std::cout << "AAA: " << *this << "\n";

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

    // auto mid = _begin + bigShift;
    // std::copy(_begin, end(), mid);
    // std::fill(_begin, mid, 0);
}

BigInt& BigInt::operator<<=(const BigInt& _rhs) {
    auto rhs = _rhs.toInt64("Can't shift left by that much");

    if (rhs < 0) throw std::logic_error("Negative shift");
    // std::cout << "PRs: " << *this << "\n";
    lshEqualsImpl(rhs, size);
    // std::cout << "res: " << *this << "\n";
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

    if (rhs < 0) throw std::logic_error("Negative shift");
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

//test combinations of { } from an array
// where res could be either a or b
BigInt& BigInt::add(Op op, const BigInt& a, const BigInt& b, BigInt& res) {
    // ::debug = a == 0x2101 && b == 0x11;
    //the mismatch of >= and > is important so that zero works properly
    bool bSign = !((op == Op::Add) ^ (b.sign >= 0));
    bool aSign = a.sign >= 0;

    // std::cout  << "+: " << (a).toInt64() <<  "," << (int) b.toInt64() << "\n";
    if (Debug){
        a.debugPrint("a: ");
        b.debugPrint("b: ");
        // std::cout << "sign: " << (int) b.sign <<  "," << (int) a.sign << "\n";
        // std::cout << "s-gn: " << (int) bSign <<  "," << (op == Op::Add) << "\n";
    }

    if (a.sign == Sign::Zero) {
        res = b;
        if (op != Op::Add) res.sign = -b.sign;
        //else res.sign = -b.sign;
        // return addImpl<Op::Add>(a, b, res);
        return res;
    }

    if (b.sign == Sign::Zero) {
        if (Debug){ std::cout << "BZERO\n"; }
        if (Debug){ std::cout << "s:" << (int)a.sign << "\n"; }
        res = a;
        if (Debug){ std::cout << "s:" << (int)a.sign << "\n"; }
        return res;
    }

    if (aSign == bSign) {
        // if (::debug)
        //     std::cout << "GOES TO IMPL\n";
        res.sign = a.sign;
        // std::cout << "!sizes: " << a.size << ", "<< b.size << "\n";
        // std::cout << "!signs: " << (int)a.sign << ", "<< (int)b.sign
        //           << "; " << (int) res.sign << "\n";
        if (a.size > b.size) {
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
BigInt& addImpl(const BigInt& a, const BigInt& b, BigInt& res,
                size_t aSize, size_t bSize) {

    // redundant zeroes will be removed
    if constexpr (Op == BigInt::Op::Add)
        res.resize(aSize + 1);
    else res.resize(aSize);
    // std::cout << ::debugAdd << "OI!\n";
    // debug = true;
    // bool debug = a.capacity == 9 && b.capacity == 9;
    // bool debug = BigInt::Debug;
    bool debug = false;

    if (debug) {
        // a.debugPrint("a: ");
        // b.debugPrint("b: ");
        // res.debugPrint("r: ");
        // std::cout << "a: "<< a << "\n";
        // std::cout << "b: "<< b << "\n";
        // std::cout << "r: "<< res << "\n";
        // std::cout << "==========: \n";
    }

    if (debug)
        std::cout << "sizes: " << aSize << ", "<< bSize << "\n";
    if (debug)
        std::cout << "nsizes: " << res.size << "\n";
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

        // if (debug)
        //     std::cout << "ADDI\n";
        if (aSize > i) {
            if (carry) {
                if (debug)
                    std::cout << "OIDĄ\n";
                for (;;) {
                    // we could do it without the check, but let's be safe
                    if (i >= aSize+1)
                        goto koniec;
                    res._begin[i] = a._begin[i] + carry;
                    auto rbyte = res._begin[i];
                    // auto abyte = a._begin[i];
                    // std::cout <<"z: " << i << ":"
                    //           << std::hex << (int) rbyte << "-"
                    //           << std::hex << (int) abyte << "\n";
                    ++i;
                    // if (abyte != 0xFF) break;
                    if (rbyte != 0)
                        break;
                }
                // while (i < aSize && a._begin[i] == 0) {
                //     std::cout << "POI\n";
                //     res._begin[i++] = 0xFF;
                // }

                // std::cout << "resz:" << res << "\n";
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
        // res.debugPrint("END OF ADD:");
    }
    else {
        // std::cout  << "MINUS\n";
        for (; i < bSize; ++i) {
            int16_t r = a._begin[i] - b._begin[i] - carry;
            if (r < 0) {
                carry = 1;
                // std::cout << "B: " << r << "\n";
                r = 0x0100 + r;
            }
            else carry = 0;
            res._begin[i] = (byte) r;// carry;
        }

        if (carry) {
            for (;;) {
                //we could do it without the check, but let's be safe
                if (i >= aSize) goto koniec;
                auto abyte = a._begin[i];
                res._begin[i] = a._begin[i] - carry;

                ++i;
                if (abyte != 0) break;
            }
        }
        // else {
        for (; i < aSize; ++i) {
            res._begin[i] = a._begin[i];
        }
    }
    // std::cout << "res: "<< res << "\n";
koniec:
    if (debug) res.debugPrint("PREND: ");

    // res.debugPrint("BEZ-KONIECZNY:");
    res.clearZeroes();
    if (debug) res.debugPrint("END: ");
    // res.debugPrint("KONIECY:");
    return res;
}


//https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
//for simplicity we use the form:
//a = q * b + r
//do smth like phony_q
void BigInt::divRem(const BigInt& a, const BigInt& b,
                    BigInt& q, BigInt& r) {
    
    // auto pyTest = [&](std::string_view msg,
    //                   const BigInt& rs,
    //                   const std::string& cmd)  {
    //     return;
    //     FILE* fp = popen(concat("python3 -c 'print(", cmd,
    //                             ", end=\"\")'")
    //                      .c_str(), "r");
    //     char buff[1024];

    //     fgets(buff, sizeof(buff), fp);
    //     auto expected =  rs.toDec();
    //     if (expected != buff) {
    //         std::cerr <<  "EU!!" << msg << ": '" << expected
    //                   << "'!='" << buff << "'\n";
    //         // std::cout << idt;
    //         rs.debugPrint("res: ");
    //         pclose(fp);
    //         std::cerr << "zły-koniec świata\n";
    //         exit(-1);
    //         return;
    //     }
    //     pclose(fp);
    // };
    if (b == 0) throw std::logic_error("Division by 0");
    // std::cout << "!sizes: " << a.size << ", "<< b.size << "\n";
    //a fairly dumb test, but we do it just in case
    if (b._begin == r._begin || q._begin == b._begin)
        throw std::logic_error("divRem: b, q and r must be references to"
                               " different objects");

    if (absCmp(b, a) > 0) {
        // std::cout << "EARLY QUIT\n";
        // if (b > a) {
        //we don't do q = 0 first because it might be the same data as a
        r = a;
        //r.debugPrint("r: ");
        q = 0;
        // q.debugPrint("q: ");
        return;
    }
    size_t bSize = b.size;
    size_t aSize = a.size;

    size_t rSize = bSize+1;
    // size_t qSize = aSize-bSize + 1;//log(a/b) = log(a) - log(b)
    size_t qSize = aSize;

    auto rSign = a.sign;
    auto qSign = a.sign * b.sign;

    // we have r < b, so we alocate memory for the worst case
    r.resize(rSize);
    for (auto& bb: r) bb = 0;
    // r.size = 0;

    if (Debug)
        q.debugPrint("iq:");
    //r = 0;
    //if (a._begin != q._begin) {
    q.resize(qSize);//log(a/b) = log(a) - log(b)
    // q.size = 0;

    // std::cout << "r+:" << r << "\n";
    // std::cout << "q+:" << q << "\n";
    //r.size = b.size-1
    auto absCmpRemainder = [&](const BigInt& r, const BigInt& b) {
        if (r._begin[bSize] != 0) {
            // std::cout << "oIII\n";
            return 1;
        }
        for (ssize_t i = bSize-1; i >= 0; --i) {
            // std::cout << "i" << i << "a: " << std::hex << (int)r._begin[i]
            //           << " b: " << (int) b._begin[i] << "\n";
            if (r._begin[i] < b._begin[i]) {
                return -1;
            }
            if (r._begin[i] > b._begin[i]) {
                return 1;
            }
        }
        return 0;
    };
    // std::cout<< "cmp: "<< absCmpRemainder(0x108, 195) << "\n";
    // std::cout<< "cmp: "<< absCmpRemainder(0x008, 195) << "\n";
    // std::cout<< "cmp: "<< absCmpRemainder(0x0218, 195) << "\n";

    // return;
    //}

    r.sign = rSign;
    q.sign = qSign;
    // if (rSize >= 10) {
    //     std::cout << "!p1: " << aSize << ", "<< bSize << "\n";
    //     std::cout << "!p2: " << rSize << ", "<< qSize << "\n";
    // }
    // std::cout << "q=:" << r << "\n";
    // for (ssize_t i = a.size-1; i >= 0; ++i) {
    if (Debug) {
        q.debugPrint("-q:");
        a.debugPrint("a:");
        b.debugPrint("b:");
        r.debugPrint("r:");
    }

    for (ssize_t i = aSize -1; i >= 0; --i) {
        auto *ptr = q._begin + i;
        byte abyte = a._begin[i];
        *ptr = 0;
        for (int bitIndex = 7; bitIndex >= 0; --bitIndex) {
            // std::cout << "rsz: " << r.size << "\n";
            r.lshEqualsImpl(1, bSize);

            r._begin[0] |= (abyte >> bitIndex) & 1;
            // auto cmp  = absCmpRemainder(r, b);
            // if (cmp != (r.toInt64() >= b.toInt64())) {
            // if (r._begin[1])
            //     std::cout << "OI:" << cmp << ":" << std::hex << r << " " << b.toInt64() << "\n";
            // }
            if (absCmpRemainder(r, b) >= 0) {
                // std::cout << "BIGR:" << ":" << std::hex << r << " " << b.toInt64() << "\n";
                // std::cout << "BIGR:" << ":" << std::hex << r << " " << b.toInt64() << "\n";
                addImpl<Op::Sub>(r, b, r, rSize, bSize);
                // set bit
                *ptr |= (1 << bitIndex);
            }
            // else {
            //     // unset bit
            //     //std::cout << "UNSET\n";
            //     *ptr &= ~(1 << bitIndex);
            // }
            // std::cout << "q>:" << q << "\n";
        }
    }
    // std::cout << "LAST :" << std::hex << r << " " << b.toInt64() << "\n";
    // r.resize(rSize);
    // q.resize(qSize);
    r.sign = rSign;
    q.sign = qSign;
    // r.resize(bSize);
    // std::cout << "q-:" << q << "\n";
    // std::cout << "r-:" << r << "\n";
    r.clearZeroes();
    q.clearZeroes();
    // std::cout << "end_div\n";
}
BigInt& BigInt::mul(const BigInt& a, const BigInt& b, BigInt& res) {
    // std::cout << "signs: " << (int)a.sign << "*"<< (int)b.sign << "="
    //           << (int)res.sign << "\n";
    // std::cout << "mul: " << (int)a << "*"<< (int)b.sign << "="
    //           << (int)res.sign << "\n";
    auto sign = a.sign* b.sign;
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
        // std::cout << "carry? "<< i <<"\n";
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
BigInt& BigInt::mulImpl(const BigInt& a, const BigInt& b, BigInt& res,
                        int level) {
    std::string idt = std::string(level*2, ' ')+"|";
    // std::cout << idt; a.debugPrint("a: ");
    // std::cout << idt; b.debugPrint("b: ");
    //           << "++mulImpl: " << std::hex << (a) << "*" << int(b) << "\n";


    auto pyTest = [&](std::string_view msg,
                      const BigInt& rs,
                      const std::string& cmd)  {
        return;
        FILE* fp = popen(concat("python3 -c 'print(", cmd,
                                ", end=\"\")'")
                         .c_str(), "r");
        char buff[1024];

        fgets(buff, sizeof(buff), fp);
        auto expected =  rs.toDec();
        if (expected != buff) {
            std::cerr << idt << "EU!!" << msg << ": '" << expected
                      << "'!='" << buff << "'\n";
            std::cout << idt;
            rs.debugPrint("res: ");
            pclose(fp);
            std::cerr << "zły-koniec świata\n";
            exit(-1);
            return;
        }
        pclose(fp);
    };
    auto py = [&](std::string_view msg, const BigInt& rs) {
        return;
        FILE* fp = popen(concat("python3 -c 'print(", a.toDec(), "*", b.toDec(),
                                ", end=\"\")'")
                         .c_str(), "r");
        char buff[1024];

        fgets(buff, sizeof(buff), fp);
        auto expected =  rs.toDec();
        if (expected != buff) {
            std::cerr << idt << "E!!" << msg << ": '" << expected
                      << "'!='" << buff << "'\n";
            std::cout << idt;
            rs.debugPrint("res: ");
            pclose(fp);
            std::cerr << "koniec świata\n";
            exit(-1);
            return;
        }
        pclose(fp);
    };


    if (a.size <= 1) {
        smallMulImpl(b, a._begin[0], res);
        py("smol-a", res);

        return res;
    }
    if (b.size <= 1) {
        smallMulImpl(a, b._begin[0], res);
        py("smol-b", res);

        return res;
    }
    size_t m = std::min(a.size, b.size);
    size_t m2 = m/2;

    BigInt lowA, highA, lowB, highB;

    a.splitNum(lowA, highA, m2);
    b.splitNum(lowB, highB, m2);
    // std::cout << idt<< "a: "<<std::hex<<int(a) << " " << int(lowA)<<"|"<< int(highA)<<"\n";
    // std::cout << idt<< "b: "<<std::hex<<int(b) << " " << int(lowB)<<"|"<< int(highB)<<"\n";

    BigInt& z2 = res;
    BigInt z0, z1;
    // std::cout<< idt<< "m2: "<< m2 << "\n";
    // int ila = int(lowA), ilb = int(lowB), iha = int(highA), ihb = int(highB);
    // std::cout << idt << "HIGH:\n";
    mulImpl(highA, highB, z2, level+1);
    // std::cout << "LOW:\n";
    mulImpl(lowA, lowB, z0, level+1);


    // if (int(z0) != ila*ilb) std::cerr << idt << "===AHA- z0\n";
    // if (int(z2) != iha*ihb) {
    //     std::cerr << idt<< "===AHA- z2\n";
    //     std::cout << idt<< "!aka: " << iha << "*" << ihb
    //               << "got:" << int(z2) << "\n";
    // }

    // std::cout << "BOTH:\n";
    // mulImpl(lowA+highA, lowB+highB, z1);
    lowA += highA;
    lowB += highB;
    // if (iha+ila != int(lowA)) std::cerr << idt<< "===AHA- RA\n";
    // if (ihb+ilb != int(lowB)) std::cerr << idt<< "===AHA- RB\n";
    // std::cout << "exected: " << int(ra) << " " << int(rb)<<"\n";
    // std::cout << "after: " << int(lowA) << " " << int(lowB)<<"\n";
    mulImpl(lowA, lowB, z1, level+1);

    // std::cout << idt << "z0: " << z0 << "\n";
    // std::cout << idt << "z1: " << z1 << "\n";
    // std::cout << idt << "z2: " << z2 << "\n";

    auto z1s = z1.toDec();
    auto z2s = z2.toDec();
    auto z0s = z0.toDec();

    z1 -= z2;
    z1 -= z0;

    pyTest("z1 -=", z1, concat(z1s, "-", z2s, "-", z0s));
    z1s = z1.toDec();
    // if (int(z1) != iz1 - iz0 - iz2)
    //     std::cerr << idt<<"===AHA- z1 -= ....\n";
    // std::cout << "z2 " <<int(z2) << "\n";

    // if (level == 0) {
    //     z2.debugPrint("z2b:");
    // }
    z2 <<= m2*2*8;
    // if (level == 0) {
    //     std::cout << "OIII \n";
    //     z2.debugPrint("z2a:");
    // }
    pyTest("z2 <<=", z2, concat(z2s, "<<", m2*2*8));
    z2s = z2.toDec();

    // std::cout << "ż2 " <<int(z2) << "\n";

    z1 <<= m2*8;

    pyTest("z1 <<=", z1, concat(z1s, "<<", m2*8));
    z1s = z1.toDec();

    // std::cout << idt << "-\n";
    // std::cout << idt << "Az0: " << z0 << "\n";
    // std::cout << idt << "Az1: " << z1 << "\n";
    // std::cout << idt << "Az2: " << z2 << "\n";


    z2 += z1;
    pyTest("z2 += z1", z2, concat(z2s, "+", z1s));
    z2s = z2.toDec();
    // if (z2.sign != Sign::Positive) std::cerr << idt<<"+++=Z1\n";
    // std::cout<< idt; z0.debugPrint("z0: ");
    // std::cout <<idt; z2.debugPrint("z2: ");
    // if (level == 1)
    //     ::debugAdd = true;
    z2 += z0;
    pyTest("z2 += z0", z2, concat(z2s, "+", z0s));
    // if (level == 1)
    //     ::debugAdd = false;
    // std::cout <<idt; z2.debugPrint("res: ");
    // if (z2.sign != Sign::Positive) std::cerr << idt<< "++END\n";

    // if (int(z2) !=  iz2 +iz1 + iz0) {
    //     std::cerr << idt<<"===AHA- z2+z1+z0\n";
    //     // std::cout << idt<< "!aka: " << iz2 << "+" <<iz1 << "+" <<iz0<< "=="
    //     //           << (iz2+iz1+iz0) << "!=" << (z2)
    //     //           <<"\n";
    // }
    py("end", res);
    // std::cout << idt<< "GOT " << res << "\n"
    //           << idt;
    // res.debugPrint("Koniec::");
    return res;
}

void BigInt::debugPrint(std::string_view msg, std::ostream& s) const {
    auto& bi = *this;
    s << msg << "[";
    bi.toHex(s);
    // s << "\n<usingBuffer: " << bi.isUsingBuffer() << ">\n";
    s << "<size: " << std::dec << bi.size << ">";
    s << "<capacity: " << std::dec << bi.capacity << ">";
    // s << "<tapacity: " << std::hex << bi.theCapacity() << ">";
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
    // res.resize(0);
    // std::cout << "begin: " << res << "\n";
    res = 0;
    // std::cout << "Begin: " << res << "\n";
    auto c = s.get();
    Sign sign = Sign::Positive;

    while (!s.eof() && isspace(c)) c = s.get();

    // std::cout << "read: '" << (char)c << "'\n";

    switch (c) {
    case '-':
        sign = Sign::Negative;
        [[fallthrough]];
    case '+':
        c = s.get();
        break;
    }

    for (;;) {
        // std::cout << "c: '" << (char)c << "'\n";
        unsigned digit = c - '0';
        if (digit >= 10)
            throw std::runtime_error("Invalid BigInt");
        // std::cout << "l-yeet: " << res << "\n";
        BigInt::smallMulImpl(res, 10, res);
        res += digit;
        c = s.get();

        if (c == std::char_traits<char>::eof() || isspace(c))
            break;
    }
    res.sign = sign;

    return s;
}
