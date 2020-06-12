#include <array>
#include <iostream>

template<size_t N = 5, bool Lazy = true>
class Nton {
    int index = 0;
    int val = 0;

    explicit Nton(int index, int val = 0) : index(index), val(val) {
        std::cout << "ctor("<< index <<", " << val <<")\n";
    }


    Nton() = delete;
    Nton(const Nton&) = delete;
    Nton(Nton&&) = default;// = delete;
    Nton& operator=(const Nton&) = delete;
    Nton& operator=(Nton&&) = default;// = delete;
public:

    int getVal() const { return val; }
    void setVal(int val) { this->val = val; }

    static Nton& instance() {
        static size_t i = 0;
        union NtonUnion {
            // it's important that this is first
            char unused;
            Nton n;
        };

        static auto array = [] () {
            std::array<NtonUnion, 5> res = {};
            if constexpr (!Lazy) {
                for (size_t i = 0; i < N; ++i)
                    new (&res[i].n) Nton(i, i);
            }
            return std::move(res);
        }();

        if constexpr (Lazy) {
            if (i < N) {
                new (&array[i].n) Nton(i, i);
            }
        }
        return array[i++ % N].n;
    }
    friend std::ostream& operator<<(std::ostream& out, const Nton& n) {
        return out << "Nton{" << n.index << ", " << n.val << "}";
    }
};

int main() {
    using Fiveton = Nton<5>;
    // std::cout << "n: " <<
    std::cout << "Begin\n";
    Fiveton::instance().setVal(42);
    auto& v = Fiveton::instance();
    std::cout << "first: " << v << "\n";
    std::cout << "second: " << Fiveton::instance() << "\n";

    std::cout << "\n";

    for (size_t i = 0; i < 5; ++i) {
        auto& inst = Fiveton::instance();
        std::cout << "A: " <<inst << "\n";
    }

    v.setVal(10);

    std::cout << "\n";

    for (size_t i = 0; i < 5; ++i) {
        auto& inst =  Fiveton::instance();
        std::cout << "B: " << inst << "\n";
    }
    std::cout << "last: " << Fiveton::instance() << "\n";

    std::cout << "End\n";
    return 0;
}
