#pragma once
#include "lambda.hpp"
#include <memory>
#include <stdexcept>

inline std::unique_ptr<term> number_to_lambda(unsigned int n) {
    size_t f = new_name();
    size_t x = new_name();
    auto val = std::make_unique<value>();
    val->variable = x;
    std::unique_ptr<term> body = std::move(val);
    for (unsigned int i = 0; i < n; ++i) {
        auto app = std::make_unique<application>();
        auto f_val = std::make_unique<value>();
        f_val->variable = f;
        app->t = std::move(f_val);
        app->s = std::move(body);
        body = std::move(app);
    }
    auto abs_x = std::make_unique<abstraction>();
    abs_x->parameter = x;
    abs_x->t = std::move(body);
    auto abs_f = std::make_unique<abstraction>();
    abs_f->parameter = f;
    abs_f->t = std::move(abs_x);
    return abs_f;
}

inline bool is_number(const term *node) {
    auto abs_f = dynamic_cast<const abstraction *>(node);
    if (!abs_f)
        return false;
    auto abs_x = dynamic_cast<const abstraction *>(abs_f->t.get());
    if (!abs_x)
        return false;

    size_t f = abs_f->parameter;
    size_t x = abs_x->parameter;

    const term *curr = abs_x->t.get();
    while (curr) {
        if (auto v = dynamic_cast<const value *>(curr)) {
            return v->variable == x;
        }
        if (auto app = dynamic_cast<const application *>(curr)) {
            auto t_val = dynamic_cast<const value *>(app->t.get());
            if (!t_val || t_val->variable != f)
                return false;
            curr = app->s.get();
        } else {
            return false;
        }
    }
    return false;
}

inline int lambda_to_number(const term *node) {
    auto abs_f = dynamic_cast<const abstraction *>(node);
    if (!abs_f)
        throw std::invalid_argument("not a number");
    auto abs_x = dynamic_cast<const abstraction *>(abs_f->t.get());
    if (!abs_x)
        throw std::invalid_argument("not a number");

    size_t f = abs_f->parameter;
    size_t x = abs_x->parameter;

    const term *curr = abs_x->t.get();
    int count = 0;
    while (curr) {
        if (auto v = dynamic_cast<const value *>(curr)) {
            if (v->variable == x)
                return count;
            throw std::invalid_argument("not a number");
        }
        if (auto app = dynamic_cast<const application *>(curr)) {
            auto t_val = dynamic_cast<const value *>(app->t.get());
            if (!t_val || t_val->variable != f)
                throw std::invalid_argument("not a number");
            curr = app->s.get();
            count = count + 1;
        } else {
            throw std::invalid_argument("not a number");
        }
    }
    throw std::invalid_argument("not a number");
}
