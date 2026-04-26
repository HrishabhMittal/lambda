#include "lambda.hpp"
#include "parser.hpp"
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
    while (true) {
        auto res = parseStatement(file);
        if (res.type == ResultType::EXPR) {
            evaluate(res.t);
            if (is_number(res.t.get())) {
                std::cout<<lambda_to_number(res.t.get())<<std::endl;
            } else {
                res.t->print();
                std::cout<<std::endl;
            }
        }
        if (res.type == ResultType::RT_EOF)
            break;
    }
    return 0;
}
