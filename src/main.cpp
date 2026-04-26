#include "lambda.hpp"
#include "util.hpp"
#include <fstream>

int main(int argc, char **argv) {
    if (argc != 2) {
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        return 2;
    }

    auto ast = parse_term(file);
    if (!ast) {
        return 3;
    }

    for (int i = 0; i < 10; i++) {
        std::cout << "step " << i << ": ";
        ast->print();
        std::cout << std::endl;
        if (!step(ast))
            break;
    }

    return 0;
}
