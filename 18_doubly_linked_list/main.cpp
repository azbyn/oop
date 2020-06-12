#include "doubleList.h"
#include "managedPtr.h"

class Person {
    std::string name;
    int age;

public:
    Person(const std::string& name, int age)
            : name(name), age(age) {}

    int getAge() const { return age; }
    const std::string& getName() const { return name; }

    friend std::ostream& operator<<(std::ostream& s, const Person& p) {
        return p.print(s);
        // s << "{";
        // return p.print(s) << "}";
    }
    virtual ~Person() noexcept = default;

protected:
    virtual std::ostream& print(std::ostream& s) const {
        return s << name << ", " << age << " years old";
    }
};

class Student : public Person {
    int year;
    std::string facultyName;

public:
    Student(const std::string& name, int age,
            int year, const std::string& facultyName)
            : Person(name, age), year(year), facultyName(facultyName) {}

    int getYear() const { return year; }
    const std::string& getFacultyName() const { return facultyName; }

    std::ostream& print(std::ostream& s) const override {
        return Person::print(s) << ", Year " << year << " at " << facultyName;
    }
};

class EmployedStudent : public Student {
    std::string companyName;

public:
    EmployedStudent(const std::string& name, int age,
                    int year, const std::string& facultyName,
                    const std::string& companyName)
            : Student(name, age, year, facultyName), companyName(companyName) {}

    const std::string& getCompanyName() const { return companyName; }

    std::ostream& print(std::ostream& s) const override {
        return Person::print(s) << ", works at " << companyName;
    }
};

template<typename Iter, typename F>
auto average(Iter begin, Iter end, F getNum) {
    using ReturnType = decltype(getNum(*begin)); //std::invoke_result_t<F, Iter>;

    using SumType = std::conditional_t<
        std::is_integral_v<ReturnType>,
        std::conditional_t<std::is_signed_v<ReturnType>, int64_t, uint64_t>,
        ReturnType>;

    SumType sum {};

    size_t count = 0;

    for (Iter it = begin; it != end; ++it) {
        sum += getNum(*it);
        ++count;
    }

    if constexpr (std::is_integral_v<ReturnType>)
        return double(sum) / count;
    else
        return sum / count;
}

int main() {
    DoubleList<ManagedPtr<Person>> list;

    list.emplace_back(Person("Alice", 48));
    list.emplace_back(Person("Bob", 18));
    list.emplace_back(Person("John", 30));
    list.emplace_back(Student("Pablo", 20, 2, "Math"));
    list.emplace_back(EmployedStudent("Billy", 21, 3, "math", "Some Firm"));

    std::cout << "list: " << list << "\n";

    std::cout << "\nStudents:\n";
    for (auto& person : list) {
        if (person.is<Student>()) std::cout << "- " << person << "\n";
    }

    std::cout << "\nAverage age: ";
    std::cout << average(list.begin(), list.end(),
                         [] (auto& ptr) { return ptr->getAge(); });
    std::cout << "years \n";

    return 0;
}
