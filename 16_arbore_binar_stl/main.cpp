#include "binaryTree.h"

#include <iomanip>


template<typename T>
void testWith() {
    try {
        auto bt = BinaryTree<T>::fromFile("tree.txt");
        std::cout << "tree: \n" << bt << "\n";

        std::cout << "inorder: \n";
        bt.visitInOrder([] (const auto& val) { std::cout << val << " "; });
        std::cout << "\n";

        std::cout << "preorder: \n";
        bt.visitPreOrder([] (const auto& val) { std::cout << val << " "; });
        std::cout << "\n";

        std::cout << "postorder: \n";
        bt.visitPostOrder([] (const auto& val) { std::cout << val << " "; });
        std::cout << "\n";
    }
    catch (const std::exception& val) {
        std::cerr << val.what() << "\n";
        exit(-1);
    }

}

int main() {
    std::cout << "Int:\n";
    testWith<int>();

    std::cout << "\n\nString:\n";
    testWith<std::string>();

    return 0;
}
