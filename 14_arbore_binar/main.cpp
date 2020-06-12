#include "binaryTree.h"

#include <iomanip>

int main() {
    try {
        auto bt = BinaryTree<int>::fromFile("tree.txt");
        std::cout << bt << "\n";

        std::cout << "inorder: \n";
        bt.visitInorder([] (const auto& val) { std::cout << val << " "; });
        std::cout << "\n";

        std::cout << "preorder: \n";
        bt.visitPreorder([] (const auto& val) { std::cout << val << " "; });
        std::cout << "\n";

        std::cout << "postorder: \n";
        bt.visitPostorder([] (const auto& val) { std::cout << val << " "; });
        std::cout << "\n";

    }
    catch (const std::exception& val) {
        std::cerr << val.what() << "\n";
        return -1;
    }

    return 0;
}
