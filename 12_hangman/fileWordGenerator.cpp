#include "fileWordGenerator.h"

#include <algorithm>
#include <codecvt>
#include <fstream>
#include <locale>
#include <sstream>


template<typename... Args>
static std::string concat(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}
static std::string string_cast(const std::wstring& s) {
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(s);
}

static std::wstring& ltrim(std::wstring& str) {
    auto it2 = std::find_if(str.begin(), str.end(), [](wchar_t ch) {
        return !std::isspace<wchar_t>(ch, std::locale::classic());
    });
    str.erase(str.begin(), it2);
    return str;
}

static std::wstring& rtrim(std::wstring& str) {
    auto it1 = std::find_if(str.rbegin(), str.rend(), [](wchar_t ch) {
        return !std::isspace<wchar_t>(ch, std::locale::classic());
    });
    str.erase(it1.base(), str.end());
    return str;
}
static std::wstring& trim(std::wstring& str) { return ltrim(rtrim(str)); }
struct Parser {
    std::wifstream f;
    std::vector<std::wstring>& words;
    size_t& maxTries;
    size_t line = 1;

    Parser(std::vector<std::wstring>& words, size_t& maxTries,
            const char* path)
            : f(path), words(words),  maxTries(maxTries) {
        enforceKeyword(L"MaxTries");
        enforceKeyword(L":");
        f >> maxTries;
        words.clear();

        std::wstring word;
        while (std::getline(f, word)) {
            trim(word);
            if (word.size())
                words.push_back(word);
        }
    }

    static constexpr bool isEof(wchar_t c) {
        return c == (wchar_t) std::char_traits<wchar_t>::eof();
    }

    bool eatSpace() {
        for (;;) {
            wchar_t c = f.get();
            if (c ==  isEof(c)) return true;
            if (c == '\n') {
                ++line;
            }
            else if (!isspace(c)) {
                f.unget();
                break;
            }
        }
        return false;
    }


    void enforceKeyword(const wchar_t* keyword) {
        eatSpace();
        const wchar_t* p = keyword;
        while (*p) {
            if ((wchar_t)f.get() != *p++) {
                throw std::runtime_error(
                    concat("Error at line ", line, ": expected '",
                           string_cast(keyword), "'.").c_str());
            }
        }
    };
};

void FileWordGenerator::readFile(const char* path, size_t& numTries) {
    Parser(words, numTries, path);
}
