#include "bigint.h"

#include "misc.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <tuple>

#include <exception>
#include <stdexcept>

#include <limits>
#include <random>

#include <sstream>
#include <array>

#include <experimental/source_location>

class BigIntTester {
    using SL = std::experimental::source_location;
    bool hasErrors = false;

    void error(const char* msg, SL from = SL::current()) {
        hasErrors = true;
        std::cerr << from.file_name() << ":"
                  << from.line() << ":" << from.column() <<":" <<msg<< "\n";
    }
    bool assert(bool cond, const char* msg, SL from = SL::current()) {
        if (!cond) {
            error(msg, from);
        }
        return cond;
    }

    template<typename T, typename E>
    bool assertEquals(std::string_view name, const T& val, const E& expected,
                      SL from = SL::current()) {
        if (val != expected) {
            if constexpr (std::is_integral_v<T>) {
                error(concat("Assertion failed: expected ", name,
                             " == `", std::dec, expected,
                             "` (hex `", std::hex, expected,
                             "`) got `", val, "`.").c_str(), from);
            }
            else {
                error(concat("Assertion failed: expected ", name,
                             " == `", std::dec, expected,
                             "` got `", val, "`.").c_str(), from);
            }
            return false;
        }
        return true;
    }

    template<typename ExceptionType, typename F>
    bool assertThrows(F f, SL from = SL::current()) {
        try {
            f();
        }
        catch (const ExceptionType&) {
            return true;
        }
        error("Expected exception, got nothing\n", from);
        return false;
    }
    template<typename ExceptionType, typename F>
    bool assertThrows(std::string_view what, F f, SL from = SL::current()) {
        try {
            f();
        }
        catch (const ExceptionType& e) {
            assertEquals("Expected exception of with msg", e.what(), what, from);
            return true;
        }
        error("Expected exception, got nothing\n", from);
        return false;
    }

    bool assertBuffer(const BigInt& val, size_t size, SL from = SL::current()) {
        return assertEquals("isUsingBuffer", val.isUsingBuffer(), true, from) &
            assertEquals("size", val.size, size,from);
    }
    bool assertNotBuffer(const BigInt& val, size_t size, size_t capacity,
                         SL from = SL::current()) {
        return assertEquals("isUsingBuffer", val.isUsingBuffer(), false, from) &
            assertEquals("size", val.size, size, from) &
            assertEquals("capacity", val.capacity, capacity, from);
    }
    std::mt19937 gen {std::random_device()()};

    std::uniform_int_distribution<int64_t> dis {
        //divide by 2 to avoid overflow
        std::numeric_limits<int64_t>::min()/2,
        std::numeric_limits<int64_t>::max()/2
    };

    int64_t randomInt() { return dis(gen); }

public:
    static void test() {
        BigIntTester t;
        t.testResizeMove();
        t.testClearZeroes();
        t.testOr();
        t.testAnd();
        t.testLsh();
        t.testRsh();
        t.testCmp();
        t.testToInt64();
        t.testAbsCmp();
        t.testAdd();
        t.testDiv();
        t.testSmallMul();
        t.testSplitNum();
        t.testMul();

        t.testFromStr();
        t.testToStr();
        t.testIo();
        t.testPy();
    }
private:
    using byte = BigInt::byte;

    void testPy() {
        auto py = [&](std::string_view a, std::string_view op,
                      std::string_view b, const std::string& expected,
                      SL from = SL::current()) {
            FILE* fp = popen(concat("python3 -c 'print(", a, op, b,
                                    ", end=\"\")'")
                             .c_str(), "r");
            char buff[1024];

            fgets(buff, sizeof(buff), fp);
            if (!assertEquals(op, expected, buff, from)) {
                std::cout << "a:" << a << "\n";
                std::cout << "b:" << b << "\n";
            }

            pclose(fp);
        };
        auto t = [&](std::string_view as,
                     std::string_view bs,
                     SL from = SL::current()) {
            BigInt a {as};
            BigInt b {bs};

            BigInt res = (a*b);
            py(as, "*", bs, res.toDec(), from);
            res = (a/b);
            py(as, "//", bs, res.toDec(), from);

            res = (a%b);
            py(as, "%", bs, res.toDec(), from);
            res = (a-b);
            py(as, "-", bs, res.toDec(), from);
            res = (a+b);
            py(as, "+", bs, res.toDec(), from);
        };

        char buffA[128];
        char buffB[128];
        auto randString = [&] (char* buff) {
            size_t j = 0;
            const char* cmp = buff;
            switch (rand() % 3) {
            case 0: buff[j++] = '+'; ++cmp; break;
            // case 2: buff[j++] = '-'; break;
            }
            size_t len = 10 + (rand() % 100);

            buff[j++] = '1' + (rand() % 9);

            for (; j < len; ++j) {
                buff[j] = '0' + (rand() % 10);
            }
            buff[j] = 0;
        };
        for (int i = 0; i < 50; ++i) {
            randString(buffA);
            randString(buffB);
            std::cout << "A:" << buffA << "\n";
            std::cout << "B:" << buffB << "\n";
            t(buffA, buffB);
        }
    }

    void testIo() {
        auto t = [&](std::string_view fromStr,
                     std::string_view cmp,
                     SL from = SL::current()) {
            BigInt a;
            std::stringstream ss;
            ss << fromStr;
            ss >> a;

            auto res = a.toDec();

            if (!assertEquals("fromStr", res, cmp, from)) {
                std::cout << "fromStr: " << fromStr << "\n";
                std::cout << "cmp:     " << cmp << "\n";
            }
        };

        auto randT = [&] () {
            size_t j = 0;
            char buff[128];
            const char* cmp = buff;
            switch (rand() % 3) {
            case 0: buff[j++] = '+'; ++cmp; break;
            case 2: buff[j++] = '-'; break;
            }
            size_t len = 10 + (rand() % 100);

            buff[j++] = '1' + (rand() % 9);

            for (; j < len; ++j) {
                buff[j] = '0' + (rand() % 10);
            }
            buff[j] = 0;

            t(buff, cmp);
        };

        for (int i = 0; i < 1000; ++i) {
            randT();
        }
    }
    void testToStr() {
        auto t = [&](std::string_view res, long ai, SL from = SL::current()) {
            auto str = BigInt(ai).toDec();
            assertEquals("toStr", str, res, from);
        };
        t("1", 1);
        t("-1", -1);
        t("0", 0);
        t("-123456789", -123456789);
        t("123456789", 123456789);

        t("123", 123);
        t("-123", -123);
    }
    void testFromStr() {
        auto t = [&](std::string_view str, long ai, SL from = SL::current()) {
            BigInt a(str);
            if (!assertEquals("fromStr", long(a), ai, from)) {
                std::cout << "fromStr: '" << str << "':" << ai << "\n";
            }
        };

        for (auto& s: {"+", "-", "a", "sa21", "", "    ", " ", " a", "ö"}) {
            assertThrows<BigInt::Error>("Invalid BigInt",
                                             [&] { BigInt{s}; });
        }
        t("1", 1);
        t("+1", 1);
        t("-1", -1);
        t("+0", 0);
        t("-0", 0);
        t("0", 0);
        t("-123456789", -123456789);
        t("123456789", 123456789);
        t("+123456789", 123456789);


        t("  -123456789", -123456789);
        t("  123456789", 123456789);
        t("  +123456789", 123456789);

        t("123", 123);
        t("-123", -123);
    }

    void testMul() {
        auto t = [&](long ai, long bi, SL from = SL::current()) {
            BigInt a = ai;

            if (!assertEquals("*", a*bi, ai*bi, from)) {
                std::cout << "Mul: " << std::hex << ai << "*" << bi << ":"
                          << ai*bi << "\n";
            }
            else {
                a *= bi;
                if (!assertEquals("*=", a, ai*bi, from)) {
                    std::cout << "Mul: " << std::dec << ai << "*" << bi << ":"
                              << ai*bi << "\n";
                }
            }
        };

        std::array nums = {
            0, 1, -1,
            2, -2,
            3, -3,
            21, -21,
            42, -42,
            0x111, -0x111,
            0xFFF213, -0xFFF213
        };

        for (int ai : nums) {
            for (int bi : nums) {
                t(ai, bi);
            }
        }
        for (int i = 0; i < 100000; ++i) {
            short ai = randomInt();
            long bi = randomInt() / 0xFFFF;
            t(ai, bi);
        }
        for (int i = 0; i < 100000; ++i) {
            int bi = randomInt();
            int ai = randomInt();
            t(ai, bi);
        }
    }
    void testSplitNum() {
        BigInt h, l;
        BigInt(0xab'cd'ef).splitNum(h, l, 2);
        assertEquals("h:", h, 0xcd'ef);
        assertEquals("l:", l, 0xab);
    }
    void testSmallMul() {
        auto t = [&](long ai, byte bi, SL from = SL::current()) {
            BigInt a = ai;
            BigInt r;
            BigInt::smallMulImpl(a, bi, r);
            if (!assertEquals("mul", r, ai*bi, from)) {
                std::cout << "Mul: " << ai << "*" << bi << ":"
                          << ai*bi << "\n";
            }
            else {
                BigInt::smallMulImpl(a, bi, a);
                if (!assertEquals("mul", r, ai*bi, from)) {
                    std::cout << "Mul: " << ai << "*" << bi << ":"
                              << ai*bi << "\n";
                }
            }
        };
        t(0x10000000, 0xd0);
        for (int i = 0; i < 1000; ++i) {
            unsigned a = randomInt();
            byte b = randomInt();
            t(a, b);
        }
    }
    void testDiv() {
        auto t = [&](long ai, long bi, SL from = SL::current()) {
            BigInt a = ai;
            BigInt q;
            BigInt r;
            if (bi == 0) {
                assertThrows<BigInt::Error>(
                    [&] { BigInt::divRem(a, bi, q, r); });
                assertThrows<BigInt::Error>(
                    [&] { BigInt::divRem(a, bi, a, r); });
                return;
            }
            BigInt::divRem(a, bi, q, r);
            if (!assertEquals("div_q", q, ai / bi, from) |
                !assertEquals("div_r", r, ai % bi, from)) {
                std::cout <<  "DIV_Q: <" << ai << "/" << bi << ":"
                          << ai/bi << "+" << ai %bi << ">\n";
            }
            else {
                BigInt::divRem(a, bi, a, r);
                if (!assertEquals("div_q", a, ai / bi, from) |
                    !assertEquals("div_r", r, ai % bi, from)) {
                    std::cout <<  "DIV_A: <" << ai << "/" << bi << ":"
                              << ai/bi << "+" << ai %bi << ">\n";
                }
            }
        };
        auto t2 = [&](long ai, long bi, SL from = SL::current()) {
            BigInt a = ai;
            if (bi == 0) {
                assertThrows<BigInt::Error>([&] { a/= bi;});
                assertThrows<BigInt::Error>([&] { a %= bi;});
                assertThrows<BigInt::Error>([&] { a / bi;});
                assertThrows<BigInt::Error>([&] { a % bi;});
                return;
            }
            if (!assertEquals("/", a / bi, ai / bi, from)) {
                std::cout << "d: " << ai << "/" << bi << ":"
                          << ai/bi <<"\n";
            } else {
                if (!assertEquals("%", a % bi, ai % bi, from)) {
                    std::cout << "d: " << ai << "/" << bi << ":"
                              << ai/bi <<"\n";
                } else {
                    a /= bi;
                    if (!assertEquals("/=", a, ai / bi, from)) {
                        std::cout << "d: " << ai << "/" << bi << ":"
                                  << ai/bi <<"\n";
                    }
                    else {
                        a = ai;
                        a %= bi;
                        if (!assertEquals("%=", a, ai % bi, from))
                            std::cout << "d: " << ai << "/" << bi << ":"
                                      << ai/bi <<"\n";
                    }
                    a = ai;
                }
            }
        };

        t(1,1);
        std::array nums = {
            0, 1, -1,
            2, -2,
            3, -3,
            21, -21,
            42, -42,
            0x111, -0x111
        };
        for (auto ai : nums) {
            for (auto bi : nums) {
                t(ai, bi);
                t2(ai, bi);
            }
        }
        for (int i = 0; i < 100000; ++i) {
            auto ai = randomInt();
            auto bi = randomInt() / 0xFFFF;
            t(ai, bi);
            t2(ai, bi);
        }
        for (int i = 0; i < 100000; ++i) {
            auto bi = randomInt();
            auto ai = randomInt();
            t(ai, bi);
            t2(ai, bi);
        }
    }
    void testAbsCmp() {
        assertEquals("cmp", BigInt::absCmp(0xffff, 2), 1);
        assertEquals("cmp", BigInt::absCmp(2, 0xffff), -1);
        assertEquals("cmp", BigInt::absCmp(0xffff, 0xffff), 0);

        assertEquals("cmp", BigInt::absCmp(0xffff, -2), 1);
        assertEquals("cmp", BigInt::absCmp(2, -0xffff), -1);
        assertEquals("cmp", BigInt::absCmp(0xffff, -0xffff), 0);
    }
    void testAdd() {
        std::array<BigInt, 7> arr = {
            0,
            2,-2,
            0x2211, -0x2211,
            0x323232,
            -0x323232,
        };

        auto tm = [&](long ai, long bi, SL from = SL::current()) {
            if (!assertEquals("-", BigInt(ai) - BigInt(bi), ai - bi, from)) {
                std::cout << "SUB: " <<std::dec << ai << "-" << bi << ":" << ai - bi
                          << "\n";
                std::cout << "SUB: " << std::hex <<
                    std::abs(ai) << "-" << std::abs(bi) << ":" <<
                    std::abs(ai - bi)
                          << "\n";
            }
        };

        auto tp = [&](long ai, long bi, SL from = SL::current()) {
            if (!assertEquals("+", BigInt(ai) + BigInt(bi), ai + bi, from)) {
                std::cout << "ADD: " << std::dec << ai << "+" << bi << ":"
                          << ai + bi << "\n";
                std::cout << "ADD: " << std::hex << ai << "+" << bi << ":"
                          << ai + bi << "\n";
            }
        };
        tm(65580, 98);
        tp(0xd6,0x14e05);
        tm(0x10'00'00'00, 1);
        tm(0x15a03,0xcf);
        tm(199,-65536);

        for (int i = 0; i < 1000000; ++i) {
            auto ai = randomInt();
            auto bi = randomInt();
            tm(ai, bi);
            tp(ai, bi);
        }
        for (auto& a: arr) {
            auto ai = a.toInt64();
            assertEquals("neg", -a, -ai);
            for (auto& b: arr) {
                auto bi = b.toInt64();
                assertEquals("add", a + b, ai+bi);
                assertEquals("add", a - b, ai-bi);
                assertEquals("add", -a - b, -ai-bi);
                assertEquals("add", -a + b, -ai+bi);
            }
        }

        for (size_t i = 0; i < arr.size(); ++i) {
            BigInt a;
            auto ai = arr[i].toInt64();
            for (const auto& b: arr) {
                auto bi = b.toInt64();
                a = arr[i];
                assertEquals("v:", arr[i].toInt64(), ai);
                assertEquals("v2", a.toInt64(), ai);
                a += b;
                assertEquals("add", a, ai+bi);

                a = arr[i];
                a -= b;
                assertEquals("add", a, ai-bi);

                a = arr[i];
                a += -b;
                assertEquals("add", a, ai-bi);
                a = arr[i];
                a -= -b;
                assertEquals("add", a, ai+bi);
            }
        }
        BigInt a = 231;
        BigInt b = 231;
        assertEquals("add", BigInt(0x21) +BigInt(0x1200), 0x1221);
        assertEquals("add", BigInt(0x2101) +BigInt(0x11), 0x2101 + 0x11);
        assertEquals("add", a+b, 231+231);
        assertEquals("add", BigInt(13)+BigInt(-12), 1);
        assertEquals("add", BigInt(13)-BigInt(12), 1);
        assertEquals("add", BigInt(12)-BigInt(13), -1);
        assertEquals("add", BigInt(13)-BigInt(12), 1);
        assertEquals("add", BigInt(12)-BigInt(12), 0);
        assertEquals("add", BigInt(12)-BigInt(-12), 24);
        assertEquals("add", BigInt(-12)-BigInt(-12), 0);
        assertEquals("add", BigInt(-12)+BigInt(-12), -24);

        assertEquals("add", BigInt(0x21) +BigInt(-0x1212), 0x21 -0x1212);
        assertEquals("add", BigInt(-0x1212)+ BigInt(0x21), 0x21 -0x1212);
        assertEquals("add", BigInt(0x1212)-BigInt(0x12), 0x1200);
        a -= a;
        assertEquals("add", a, 0);
    }

    void testToInt64() {
        BigInt a = 0x0A'0F22;
        assertEquals("toInt64", a.toInt64(), 0x0A'0F22);
        assertEquals("toInt64", BigInt(-0x0A'0F22).toInt64(), -0x0A'0F22);
        assertEquals("toInt64", BigInt(42).toInt64(), 42);
        assertEquals("toInt64", BigInt().toInt64(), 0);
        BigInt b { 0xFF,0,0,0xFF, 1,0, 2, 1, 2 };
        assertThrows<BigInt::Error>("Number too big for int64",
                                         [&] {b.toInt64(); });
    }
    void testCmp() {
        std::array<BigInt, 7> arr = {
            0,
            2,-2,
            0x2211, -0x2211,
            0x323232,
            -0x323232,
            };
        for (auto& a : arr) {
            for (auto& b : arr) {
                assertEquals("cmp", a == b, a.toInt64() == b.toInt64());
                assertEquals("cmp", a != b, a.toInt64() != b.toInt64());
                assertEquals("cmp", a <= b, a.toInt64() <= b.toInt64());
                assertEquals("cmp", a > b, a.toInt64() > b.toInt64());
            }
        }
        BigInt a = 0x0A'0F22;
        BigInt b = 0x0A'0F22;
        BigInt c = 0x0;
        BigInt d = -0x0A'0F22;

        assertEquals("cmp", a == b, true);
        assertEquals("cmp", a == 0x0A'0F22, true);
        assertEquals("cmp", a != b, false);
        assertEquals("cmp", a <= b, true);
        assertEquals("cmp", a <= 2, false);
        assertEquals("cmp", a > 2, true);
        assertEquals("cmp", a == 2, false);
        assertEquals("cmp", a < 0xA0'ABCD, true);
        assertEquals("cmp", a < 0xABCD, false);
        assertEquals("cmp", a < 0x100ABCD, true);
        assertEquals("cmp", c <=> d, 1);

        assertEquals("cmp", a < 0x09'ABCD, false);
    }
    void testRsh() {
        std::cout << "a: " << (BigInt(0xA0F) >> 2) << "\n";
        BigInt a = 0x0A0F22;
        std::cout << "a:" << (a >> 10) << "\n";
        a >>= 2;
        std::cout << "a:" << a << "\n";
        a >>= 7;
        std::cout << "a:" << a << "\n";
        std::cout << "T: " << (BigInt(0xA0F) >> 9) << "\n";
    }
    void testLsh() {
        BigInt a = 0x0100;
        std::cout << "a:" << (a << 2) << "\n";
        a <<= 2;
        std::cout << "a:" << a << "\n";
        a <<= 7;
        std::cout << "a:" << a << "\n";
    }
    void testAnd() {
        BigInt a = 0x0122;
        std::cout << (a & 0x0300) << "\n";
        std::cout << (0x0300 & a) << "\n";

        std::cout << (a | 0x0300) << "\n";
        std::cout << (0x0300 | a) << "\n";
        std::cout << (BigInt(0x200) & a) << "\n";
        std::cout << (BigInt(0x0F) & a) << "\n";
        BigInt b { 0xFF,0,0,0xFF, 1,0, 2, 1, 2 };// 0xF000;
        BigInt c { 0xFF,0,0xFF,0xFF, 1,0, 0xFF, 1, 0 };
        std::cout << "b:" << b << "\n";
        a &= b;
        std::cout << "a:" << a << "\n";
        b &= c;
        std::cout << "c:" << b << "\n";
    }
    void testOr() {
        BigInt a = 0x0122;
        BigInt b { 0x00,0,0,0xFF, 01,0, 2, 1, 2 };// 0xF000;
        BigInt c = 1;
        std::cout << "b:" << b << "\n";
        a |= b;
        std::cout << "a:" << a << "\n";
        b |= c;
        std::cout << "c:" << b << "\n";

        std::cout << (a | BigInt(0x300)) << "\n";
        std::cout << (BigInt(0x200) | a) << "\n";
        //a |= b;
    }
    void testResizeMove() {
        BigInt b = 0x2213;
        assertBuffer(b, 2);

        b.resize(16);
        assertNotBuffer(b, 16, 16);
        b.resize(7);
        assertNotBuffer(b, 7, 16);
        BigInt a;
        assertBuffer(a, 0);
        a = b;
        assertBuffer(a, 7);
        a.resize(9);
        assertNotBuffer(a, 9, 9);
        a = b;
        assertNotBuffer(a, 7, 9);
        a.resize(9);
        assertNotBuffer(a, 9, 9);

        BigInt c(a);
        assertNotBuffer(c, 9, 9);

        BigInt e(b);
        assertBuffer(e, 7);

        c = std::move(b);
        assertNotBuffer(c, 7, 16);

        BigInt t = 2;
        BigInt f(std::move(t));
        assertBuffer(f, 1);

        BigInt d(std::move(a));
        assertNotBuffer(d, 9, 9);

    }
    void testClearZeroes() {
        BigInt a = 0x231;
        std::cout << "a0:"<< a << "\n";
        a.clearZeroes();
        std::cout << "a1:"<< a << "\n";
        a.resize(9);
        a = 0x2231;
        std::cout << "a2:"<< a << "\n";
        a.clearZeroes();
        std::cout << "a3:"<< a << "\n";
    }
};

int main(int argc, const char** argv) {
    // BigIntTester::test();

    if (argc <= 3) {
        std::cerr << "Please provide 2 numbers and an operator in this format:\n";
        std::cerr << "Num op Num\n";
        return -1;
    }

    BigInt a(argv[1]), b(argv[3]);
    std::string_view op = argv[2];

    std::cout << a<< " " << op << " " << b << " = ";

    if      (op == "+")  std::cout << a+b;
    else if (op == "-")  std::cout << a-b;
    else if (op == "<")  std::cout << (a<b);
    else if (op == ">")  std::cout << (a>b);
    else if (op == "<=") std::cout << (a<=b);
    else if (op == ">=") std::cout << (a>=b);
    else if (op == "==") std::cout << (a==b);
    else if (op == "!=") std::cout << (a!=b);
    else if (op == "<<") std::cout << (a<<b);
    else if (op == ">>") std::cout << (a>>b);
    else if (op == "|")  std::cout << (a|b);
    else if (op == "&")  std::cout << (a&b);
    else if (op == "/")  std::cout << (a/b);
    else if (op == "*")  std::cout << (a*b);
    else if (op == "%")  std::cout << (a%b);
    else {
        std::cerr << "Invalid operator\n";
        return -1;
    }
    std::cout << "\n";
    return 0;
}
