#include "cnp.h"

inline bool isLeapYear(unsigned year) {
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    if (year % 400 != 0) return false;
    return true;
}
inline unsigned getMonthLength(unsigned month, unsigned year) {
    if (month == 2) return isLeapYear(year) ? 29 : 28;
    if (month <= 7) return (month % 2 == 1) ? 31 : 30;
    return (month % 2 == 0) ? 31 : 30;
}


Cnp::Cnp(std::string_view str) {
    if (str.size() != val.size())
        throw InvalidCnp(str, "Size must be 13");
    for (size_t i = 0; i < val.size(); ++i) {
        if (str[i] < '0' || str[i] > '9') {
            throw InvalidCnp(str, concat("Invalid charcter '", str[i],  "'"));
        }
        val[i] = str[i] - '0';
    }

    int s = val[0];
    if (s < 1 || s > 8)
        throw InvalidCnp(str, concat("Invalid sex '", s, "' expected in range 1..8"));

    int year = getYear();
    int month = getMonth();
    if (month < 1 || month > 12)
        throw InvalidCnp(str, concat("Invalid month '", month, "' expected in range 1..12"));

    int day = getDay();
    int monthLength = getMonthLength(month, year);
    if (day < 1 || day > monthLength)
        throw InvalidCnp(str, concat("Invalid day '", day, "' expected in range 1..", monthLength));

    int jud = getJudetIndex();
    if (jud < 1 || jud > 52)
        throw InvalidCnp(str, concat("Invalid judet '", jud, "' expected in range 1..52"));

    int nnn = getNNN();
    if (nnn == 0)
        throw InvalidCnp(str, concat("Invalid NNN '", nnn, "' it can't be 000"));

    int checksum = 0;
    int i = 0;
    for (auto d : {2,7,9,1,4,6,3,5,8,2,7,9}) {
        checksum += d * val[i++];
    }

    checksum %= 11;
    if (checksum == 10) checksum = 1;

    int c = val[12];
    if (c != checksum)
        throw InvalidCnp(str, "Checksum failed");
}

Sex Cnp::getSex() const {
    return val[0] % 2 == 1 ? Sex::Male : Sex::Female;
}
unsigned Cnp::getYear() const {
    auto aa = substringAsInt(1, 3);
    auto centuryIndex = (val[0] - 1) /2;
    std::array years = {1900, 1800, 2000, 1900};
    return years[centuryIndex] + aa;
}
unsigned Cnp::getMonth() const {
    return substringAsInt(3, 5);
}
unsigned Cnp::getDay() const {
    return substringAsInt(5, 7);
}
unsigned Cnp::getJudetIndex() const {
    return substringAsInt(7, 9);
}
unsigned Cnp::getNNN() const {
    return substringAsInt(9, 12);
}

std::ostream& operator<<(std::ostream& s, const Cnp& cnp) {
    for (auto d : cnp.val)
        s << (char)('0'+ d);
    return s;
}
// 64 bits are enough for a 13 digit number
uint64_t Cnp::substringAsInt(int start, int end) const {
    uint64_t res = 0;
    uint64_t multiple = 1;
    for (int i = end-1; i >= start; --i) {
        res += multiple * val[i];
        multiple *= 10;
    }
    return res;
}
