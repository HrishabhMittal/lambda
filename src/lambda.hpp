#pragma once
#include <cstddef>
#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

inline size_t new_name() {
    static size_t name = 256;
    return name++;
}

inline char map_to_char(int s) {
    if (s >= 0 && s < 26)
        return s + 'A';
    s -= 26;
    if (s >= 0 && s < 26)
        return s + 'a';
    std::unreachable();
}
inline std::string map_to_str(size_t s) {
    std::string out;
    out.reserve(16);
    const int base = 52;
    while (s) {
        out += map_to_char(s % base);
        s /= base;
    }
    return out;
}
struct term {
    virtual ~term() = default;
    virtual std::set<size_t> FV() = 0;

    virtual void substitute(size_t t1, const std::unique_ptr<term> &t2) = 0;
    virtual void rename(size_t t1, size_t t2) = 0;
    virtual void print() = 0;

    virtual std::unique_ptr<term> clone() const = 0;
    virtual bool is_equal_to(const term *other, std::map<size_t, size_t> m = {}) const = 0;
};

struct value : term {
    size_t variable;

    std::set<size_t> FV() override { return {variable}; }

    void substitute(size_t t1, const std::unique_ptr<term> &t2) override {}

    void rename(size_t t1, size_t t2) override {
        if (variable == t1)
            variable = t2;
    }

    void print() override { std::cout << map_to_str(variable); }

    std::unique_ptr<term> clone() const override {
        auto val = std::make_unique<value>();
        val->variable = variable;
        return val;
    }
    bool is_equal_to(const term *other, std::map<size_t, size_t> m = {}) const override {
        auto v = dynamic_cast<const value *>(other);
        if (!v)
            return false;
        auto it = m.find(variable);
        if (it != m.end())
            return it->second == v->variable;
        return variable == v->variable;
    }
};
struct abstraction : term {
    size_t parameter;
    std::unique_ptr<term> t;

    std::set<size_t> FV() override {
        auto fv = t->FV();
        fv.erase(parameter);
        return fv;
    }

    void substitute(size_t t1, const std::unique_ptr<term> &t2) override {
        if (parameter != t1) {
            if (t2->FV().count(parameter)) {
                rename(parameter, new_name());
            }

            auto child_val = dynamic_cast<value *>(t.get());
            if (child_val != nullptr && child_val->variable == t1) {
                t = t2->clone();
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
        std::cout << "λ" << map_to_str(parameter) << ".";
        t->print();
    }

    std::unique_ptr<term> clone() const override {
        auto abs = std::make_unique<abstraction>();
        abs->parameter = parameter;
        abs->t = t->clone();
        return abs;
    }
    bool is_equal_to(const term *other, std::map<size_t, size_t> m = {}) const override {
        auto a = dynamic_cast<const abstraction *>(other);
        if (!a)
            return false;
        m[parameter] = a->parameter;
        return t->is_equal_to(a->t.get(), m);
    }
};
struct application : term {
    std::unique_ptr<term> t, s;

    std::set<size_t> FV() override {
        auto fv = t->FV();
        if (s != nullptr) {
            auto fv2 = s->FV();
            fv.insert(fv2.begin(), fv2.end());
        }
        return fv;
    }

    void substitute(size_t t1, const std::unique_ptr<term> &t2) override {
        auto tt = dynamic_cast<value *>(t.get());
        if (tt != nullptr && tt->variable == t1) {
            t = t2->clone();
        } else {
            t->substitute(t1, t2);
        }

        if (s != nullptr) {
            auto ts = dynamic_cast<value *>(s.get());
            if (ts != nullptr && ts->variable == t1) {
                s = t2->clone();
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

    std::unique_ptr<term> clone() const override {
        auto app = std::make_unique<application>();
        app->t = t->clone();
        if (s != nullptr) {
            app->s = s->clone();
        }
        return app;
    }
    bool is_equal_to(const term *other, std::map<size_t, size_t> m = {}) const override {
        auto a = dynamic_cast<const application *>(other);
        if (!a)
            return false;
        bool s_eq = true;
        if (s && a->s)
            s_eq = s->is_equal_to(a->s.get(), m);
        else if (s || a->s)
            s_eq = false;
        return t->is_equal_to(a->t.get(), m) && s_eq;
    }
};

inline std::unique_ptr<term> parse_term(std::istream &in) {
    char c;
    in >> c;
    if (in.eof())
        return nullptr;

    if (c == '^') {
        auto abs = std::make_unique<abstraction>();
        in >> c;
        abs->parameter = c;
        in >> c;
        if (c != '.')
            throw std::runtime_error(std::string("found ") + c + " expected .");

        abs->t = parse_term(in);
        return abs;
    } else if (c == '(') {
        auto app = std::make_unique<application>();
        app->t = parse_term(in);
        app->s = parse_term(in);

        in >> c;
        if (c != ')')
            throw std::runtime_error(std::string("found ") + c + " expected )");
        return app;
    } else {
        auto val = std::make_unique<value>();
        val->variable = c;
        return val;
    }
}
