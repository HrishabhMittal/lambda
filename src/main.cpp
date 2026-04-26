#include <cctype>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

size_t new_name() {
    static size_t name = 256;
    return name++;
}

char map_to_char(size_t s) {
    if (s < 256)
        return static_cast<char>(s);
    return '?';
}

struct term {
    virtual ~term() = default;
    virtual std::set<size_t> FV() = 0;
    virtual void substitute(size_t t1, std::shared_ptr<term> t2) = 0;
    virtual void rename(size_t t1, size_t t2) = 0;
    virtual void print() = 0;
};

struct value : term {
    size_t variable;

    std::set<size_t> FV() override { return {variable}; }

    void substitute(size_t t1, std::shared_ptr<term> t2) override {}

    void rename(size_t t1, size_t t2) override {
        if (variable == t1)
            variable = t2;
    }

    void print() override { std::cout << map_to_char(variable); }
};

struct abstraction : term {
    size_t parameter;
    std::shared_ptr<term> t;

    std::set<size_t> FV() override {
        auto fv = t->FV();
        fv.erase(parameter);
        return fv;
    }

    void substitute(size_t t1, std::shared_ptr<term> t2) override {
        if (parameter != t1) {

            if (t2->FV().count(parameter)) {
                rename(parameter, new_name());
            }

            auto child_val = dynamic_cast<value *>(t.get());
            if (child_val != nullptr && child_val->variable == t1) {
                t = t2;
            } else {
                t->substitute(t1, t2);
            }
        }
    }

    void rename(size_t t1, size_t t2) override {
        if (parameter == t1)
            parameter = t2;
        t->rename(t1, t2);
    }

    void print() override {
        std::cout << "λ" << map_to_char(parameter) << ".";
        t->print();
    }
};

struct application : term {
    std::shared_ptr<term> t, s;

    std::set<size_t> FV() override {
        auto fv = t->FV();
        if (s != nullptr) {
            auto fv2 = s->FV();
            fv.insert(fv2.begin(), fv2.end());
        }
        return fv;
    }

    void substitute(size_t t1, std::shared_ptr<term> t2) override {
        auto tt = dynamic_cast<value *>(t.get());
        if (tt != nullptr && tt->variable == t1) {
            t = t2;
        } else {
            t->substitute(t1, t2);
        }

        if (s != nullptr) {
            auto ts = dynamic_cast<value *>(s.get());
            if (ts != nullptr && ts->variable == t1) {
                s = t2;
            } else {
                s->substitute(t1, t2);
            }
        }
    }

    void rename(size_t t1, size_t t2) override {
        t->rename(t1, t2);
        if (s != nullptr)
            s->rename(t1, t2);
    }

    void print() override {
        std::cout << "(";
        t->print();
        std::cout << " ";
        if (s != nullptr)
            s->print();
        std::cout << ")";
    }
};

std::shared_ptr<term> parse_term(std::istream &in) {
    char c;
    in >> c;
    if (in.eof())
        return nullptr;

    if (c == '^') {
        auto abs = std::make_shared<abstraction>();
        in >> c;
        abs->parameter = c;
        in >> c;
        if (c != '.')
            throw std::runtime_error(std::string("found ") + c + " expected .");

        abs->t = parse_term(in);
        return abs;
    } else if (c == '(') {
        auto app = std::make_shared<application>();
        app->t = parse_term(in);
        app->s = parse_term(in);

        in >> c;
        if (c != ')')
            throw std::runtime_error(std::string("found ") + c + " expected )");
        return app;
    } else {
        auto val = std::make_shared<value>();
        val->variable = c;
        return val;
    }
}
bool step(std::shared_ptr<term> &node) {
    if (!node)
        return false;
    auto app = std::dynamic_pointer_cast<application>(node);
    if (app) {
        auto left_abs = std::dynamic_pointer_cast<abstraction>(app->t);
        if (left_abs) {
            auto m_val = std::dynamic_pointer_cast<value>(left_abs->t);
            if (m_val && m_val->variable == left_abs->parameter) {
                node = app->s;
            } else {
                left_abs->t->substitute(left_abs->parameter, app->s);
                node = left_abs->t;
            }
            return true;
        }
        if (step(app->t))
            return true;
        if (step(app->s))
            return true;

        return false;
    }
    auto abs = std::dynamic_pointer_cast<abstraction>(node);
    if (abs) {
        return step(abs->t);
    }
    return false;
}
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
        std::cout << "step " << i << ": " << std::endl;
        ast->print();
        std::cout << std::endl;
        if (!step(ast))
            break;
    }
    return 0;
}
