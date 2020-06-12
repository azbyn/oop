#include "person.h"
#include "doubleList.h"

// #include <algorithm> //std::max
#include <charconv>
#include <fstream>
#include <iostream>

class Exception : public std::runtime_error {
    int code_;
public:
    Exception(int code, const std::string& msg)
            : std::runtime_error(msg), code_(code) {}

    int code() const {return code_; }
};
class ExpectedException : public Exception {
public:
    ExpectedException(const std::string& msg) : Exception(42, msg) {}
};

class ParseException : public Exception{
public:
    ParseException(const std::string& msg) : Exception(53, msg) {}
    ParseException(int code, const std::string& msg) : Exception(code, msg) {}
};

struct ParsingError {
    size_t lineNumber;
    std::string line;
    std::string what;
    int code;

    ParsingError(size_t lineNumber, std::string line, const std::exception_ptr& error)
            : lineNumber(lineNumber), line(line) {
        try {
            std::rethrow_exception(error);
        } catch (const Exception& e) {
            code = e.code();
            what = e.what();
        } catch (const std::exception& e) {
            code = -1;
            what = e.what();
        }
    }

    friend std::ostream& operator<<(std::ostream& s, const ParsingError& err) {
        s << "<line>\n"
          << "\t<line_number>"<< err.lineNumber << "</line_number>\n"
          << "\t<line_text>"<< err.line << "</line_text>\n"
          << "\t<error_code>"<< err.code << "</error_code>\n"
          << "\t<error_text>"<< err.what << "</error_text>\n"
          << "</line>\n";
        return s;
    }
    void printNicely() const {
        std::cerr << "\n\033[31m"<< "Error at line " << lineNumber << "\033[0m: "
                  << what << " (code " << code << "):\n"
                  << line << "\n";
    }
};

class StudentParser {

    std::ifstream file;
    DoubleList<Student> students;
    DoubleList<ParsingError> errors;

public:
    explicit StudentParser(const std::string& path) : file(path) {
        std::string line;
        int lineNum = 1;
        while (std::getline(file, line)) {
            //ignore empty lines
            if (line.find_first_not_of(" \t\n\v\f\r") == std::string::npos)
                continue;
            try {
                students.push_back(studentFromLine(line));
            } catch (...) {
                errors.emplace_back(lineNum, line, std::current_exception());
            }
            ++lineNum;
        }
    }
    const auto& getStudents() const { return students; }
    const auto& getErrors() const { return errors; }

private:
    void trimLeft(std::string_view& str) {
        str.remove_prefix(std::min(str.find_first_not_of(' '), str.size()));
    }
    void trimRight(std::string_view& str) {
        str.remove_suffix(std::max(str.size()-str.find_last_not_of(' ')-1, 0ul));
    }
    unsigned parseUint(std::string_view s) {
        unsigned out;
        auto r = std::from_chars(s.data(), s.data() + s.size(), out);
        if (r.ec != std::errc())
            throw ParseException(
                std::make_error_code(r.ec).value(),
                concat("Can't parse '", s, "' to uint"));
        return out;
    }
    Student studentFromLine(std::string_view line) {
        std::array<std::string_view, 4> values;
        std::size_t index;
        for (size_t i = 0; i < values.size(); ++i) {
            trimLeft(line);

            index = line.find_first_of(',');
            if (index == std::string::npos && i != 3)
                throw ExpectedException(concat("Expected 4 values, got ", i+1));

            values[i] = line.substr(0, index);
            trimRight(values[i]);

            line.remove_prefix(index+1);
        }
        if (index != std::string::npos)
            throw ExpectedException(concat("Expected 4 values, got more"));

        return Student(values[0], values[1], parseUint(values[2]),
                       parseUint(values[3]));
    }
};

int main() {
    StudentParser p("students.txt");
    auto& students = p.getStudents();
    std::cout << "Students: \n";
    for (auto& s : students) {
        std::cout << s << "\n";
    }
    auto& errors = p.getErrors();

    if (!errors.empty()) {
        std::ofstream log("log.xml");
        std::cerr << "\nErrors:\n";
        for (auto& e : errors) {
            e.printNicely();
            log << e;
        }
    }
    return 0;
}
