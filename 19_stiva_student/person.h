#include "cnp.h"

class InvalidName : public std::invalid_argument {
public:
    InvalidName(std::string_view name, std::string_view why)
        : std::invalid_argument(concat("Invalid name '", name,"': ", why, ".")) {}
};

class Person {
    std::string name;
    Cnp cnp;

public:
    Person(std::string_view name, std::string_view cnp)
        : name(name), cnp(cnp) {
        for (auto c : name) {
            if (!(c == ' ' ||  (c >= 'a' && c <= 'z') || (c >= 'A' || c <= 'Z'))) {
                throw InvalidName(name, concat("Invalid character in name: '", c , "'"));
            }
        }
    }
    virtual ~Person() noexcept = default;

    const Cnp& getCnp() const { return cnp; }
    const std::string& getName() const { return name; }

    virtual std::ostream& print(std::ostream& s) const {
        return s << name << ", " << cnp;
    }

    friend std::ostream& operator<<(std::ostream& s, const Person& p) {
        return p.print(s);
    }
};

class Student : public Person {
    unsigned score;
    unsigned year;
public:
    Student(std::string_view name, std::string_view cnp,
            unsigned score, unsigned year)
        : Person(name, cnp), score(score), year(year) {}

    unsigned getScore() const { return score; }
    unsigned getYear() const { return year; }

     std::ostream& print(std::ostream& s) const override {
         return Person::print(s) << ", "<< score << ", " << year;
    }
};
