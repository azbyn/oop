#include "utils.h"
#include "ptrRange.h"
#include <fstream>
#include <conio.h>

template<typename T>
struct BinaryTree {
    using Node = int;
    size_t numNodes;
    ManagedPtrRange<Node> left;
    ManagedPtrRange<Node> right;
    BinaryTree() : left(nullptr), right(nullptr) {}
    BinaryTree(size_t numNodes) : left(numNodes), right(numNodes) {}

    struct Parser {
        std::ifstream f;
        BinaryTree& res;
        size_t numNodes;
        size_t line;
        Parser(const char* path, BinaryTree& res) : f(path, std::ios::in | std::ios::binary), res(res) {
            body(readNumNodes());
        }
        size_t readNumNodes() {
            enforceKeyword("nodes");
            enforceKeyword(":");
            size_t res = enforceVal<size_t>("Integer expected");
            enforceKeyword(";");
            return res;
        }
        void body(size_t numNodes) {
            std::cout << "nn: " << numNodes << "\n";
            res = BinaryTree_Vector(numNodes);
            for (Node i = 0; i < numNodes; ++i) {
                res.left[i] = -1;
                res.right[i] = -1;
            }
            for (;;) {
                if (eatSpace()) return;
                readTreeChildren();
            }
        }
        void readTreeChildren() {
            Node n = enforceNode("Expected node (int)");
            enforceKeyword(":");
            if (checkChar(';')) return;
            if (!checkChar('_')) {
                res.left[n] = enforceNode("Expected child");
            }
            if (checkChar(',')) {
                if (!checkChar('_')) {
                    res.right[n] = enforceNode("Expected child");
                }
            }
            enforceKeyword(";");
        }
        //returns is eof
        bool eatSpace() {
            for (;;) {
                char c = f.get();
                if (isEof(c)) return true;
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
        /*void debugPrintC(int n=3) {
        printf("dbg: '");

        for (int i = 0;i<n; ++i)
        printf("%c", f.get());
        printf("'\n");
        for (int i = 0;i<n; ++i)
        f.unget();
        }*/

        bool checkChar(char c) {
            eatSpace();

            if (f.get() != c) {
                f.unget();
                return false;
            }
            return true;
        }
        void enforceKeyword(const char* msg) {
            eatSpace();
            const char* p = msg;
            while (*p) {
                if (f.get() != *p++) {
                    expectedErr(msg);
                    ///return false;
                }
            }
        }
        void debugPrintC(int n = 3) {
            std::cout << "debug: '";
            for (int i = 0; i < n; ++i)
                std::cout << (char)f.get();
            for (int i = 0; i < n; ++i)
                f.unget();
            std::cout << "'\n";
        }
        template<typename T>
        T enforceVal(const char* msg) {
            eatSpace();
            T res{};
            if (!(f >> res)) syntaxError(msg);
            return res;
        }
        Node enforceNode(const char* msg) {
            eatSpace();

            Node res{};


            char c = tolower(f.get());
            //fprintf(stderr, "Got node %d,(%c)\n ", c, c);
            if (c >= 'a' && c <= 'z') return c - 'a';
            if (f >> res) {
                return res;
            }
            else
                syntaxError(msg);

        }
        void expectedErr(const char* msg) {
            std::cerr << "Syntax error on line " << line << ": "
                << "Expected '" << msg << "', got"
                << "'" << char(f.get()) << "'" << "\n";
            throw std::runtime_error("syntax error");
        }
        void syntaxError(const char* msg) {
            std::cerr << "Syntax error on line " << line << ": "
                << msg << "\n";
            throw std::runtime_error("syntax error");
        }
        constexpr static bool isEof(char c) {
            return c == std::char_traits<char>::eof();
        }
    };
    static BinaryTree fromFile(const char* path) {
        BinaryTree res;
        Parser p(path, res);
        return res;
    }
    /*
    static void print(std::ostream& s, const BinaryTree* t, size_t indent = 0) {
        for (size_t i = 0; i < indent * 2; ++i) std::cout << ' ';
        if (t == nullptr) {
            std::cout << "null\n";
            return;
        }
        std::cout << (char)(t->n + 'A') << "\n";
        if (t->left == nullptr && t->right == nullptr) return;
        print(s, t->left, indent + 1);
        print(s, t->right, indent + 1);
    }*/

    friend std::ostream& operator<<(std::ostream& s, const BinaryTree& t) {
        s << "left:" << t.left << "\n";
        return s << "right:" << t.right << "\n";
    }


    template<typename F>
    void visitPreorder(F visitor, Node n = 0) const {
        if (n == -1) return;
        visitor(n);
        visitPreorder(visitor, left[n]);
        visitPreorder(visitor, right[n]);
    }
    template<typename F>
    void visitInorder(F visitor, Node n = 0) const {
        if (n == -1) return;
        visitInorder(visitor, left[n]);
        visitor(n);
        visitInorder(visitor, right[n]);
    }
    template<typename F>
    void visitPostorder(F visitor, Node n = 0) const {
        if (n == -1) return;
        visitPostorder(visitor, left[n]);
        visitPostorder(visitor, right[n]);
        visitor(n);
    }

    template<typename F>
    void visitBreadthFirst(F visitor, Node n = 0) {
        Vector<Node> q = { n };
        while (!q.empty()) {
            Node curr = q.front();
            q.pop_front();
            visitor(curr);
            if (left[curr]!= -1) q.push_back(left[curr]);
            if (right[curr]!=-1) q.push_back(right[curr]);
        }
    }
    size_t getHeight(Node n = 0, size_t level = 0) {
        //std::cout << t->n << ": " << level << "\n";
        size_t max = level;
        auto check = [&](size_t val) { if (max < val) max = val; };
        if (left[n] != -1) check(getHeight(left[n], level + 1));
        if (right[n]!=-1) check(getHeight(right[n], level + 1));

        return max;
    }
    size_t terminalNodes(Node n = 0) {
        if (left[n]!=-1) {
            auto l = terminalNodes(left[n]);
            if (right[n] != -1) {
                return l + terminalNodes(right[n]);
            }
            return l;
        }
        else if (right[n] != -1) {
            return terminalNodes(right[n]);
        }
        std::cout << (char)(n + 'A') << " ";
        return 1;
    }
};

void printOnLevel(const BinaryTree* t,size_t searchedLevel, size_t level = 0) {
    //std::cout << t->n << ": " << level << "\n";
    if (searchedLevel == level) {
        std::cout << (char)(t->n + 'A') << " ";
        return;
    }
    if (t->left) printOnLevel(t->left, searchedLevel, level + 1);
    if (t->right) printOnLevel(t->right, searchedLevel, level + 1);
}

int main() {
    auto t = BinaryTree_Vector::fromFile("graph.txt");
    std::cout << t;
    std::cout << "\nPreorder:\n";
    auto visitor = [](BinaryTree_Vector::Node n) { std::cout << (char)(n+'A') << " "; };

    t.visitPreorder(visitor);
    std::cout << "\nInorder:\n";
    t.visitInorder(visitor);
    std::cout << "\nPostorder:\n";
    t.visitPostorder(visitor);
    std::cout << "\nBreadth First:\n";
    t.visitBreadthFirst(visitor);
    std::cout << "\n\n";
    std::cout << "Height: " << t.getHeight() << "\n";
    std::cout << "Terminal nodes: \n";
    auto tn1 = t.terminalNodes();
    std::cout << "\nTerminal nodes count: " << tn1 << "\n";

    std::cout << "\nwith references:\n";
    auto t2 = BinaryTree::fromFile("graph.txt");

    auto visitor2 = [](const BinaryTree* t) { std::cout << (char)(t->n+'A') << " "; };
    visitPreorder(t2, visitor2);
    std::cout << "\nInorder:\n";
    visitInorder(t2, visitor2);
    std::cout << "\nPostorder:\n";
    visitPostorder(t2, visitor2);
    std::cout << "\nBreadth First:\n";
    visitBreadthFirst(t2, visitor2);
    std::cout << "\n\n";
    std::cout << "Height: " << getHeight(t2) << "\n";
    std::cout << "Terminal nodes: \n";
    auto tn = terminalNodes(t2);
    std::cout << "\nTerminal nodes count: " << tn << "\n";
    size_t l = 2;
    std::cout << "On level " << l << ":\n";
    printOnLevel(t2, l);


    _getch();
    return 0;
}
