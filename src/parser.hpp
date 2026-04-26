#pragma once
#include "lambda.hpp"
#include "util.hpp"
#include <algorithm>
#include <istream>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

inline std::map<std::string, std::string> replace;

inline std::string expand(std::string s, std::set<std::string> seen = {}) {
    std::regex macro("#[^#]+#");
    std::smatch match;
    while (std::regex_search(s, match, macro)) {
        std::string m = match.str();
        if (seen.count(m)) {
            throw std::runtime_error("recursion detected for macro: " + m);
        }
        if (replace.find(m) == replace.end()) {
            throw std::runtime_error("undefined macro: " + m);
        }

        seen.insert(m);

        std::string expanded_m = expand(replace[m], seen);
        seen.erase(m);

        s.replace(match.position(), match.length(), expanded_m);
    }

    std::regex num("\\d+");
    std::string res;
    size_t last_pos = 0;
    auto words_begin = std::sregex_iterator(s.begin(), s.end(), num);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch m = *i;
        res += s.substr(last_pos, m.position() - last_pos);

        int n = std::stoi(m.str());
        std::string body = "x";
        for (int j = 0; j < n; ++j) {
            body = "(f " + body + ")";
        }

        res += "^f.^x." + body;

        last_pos = m.position() + m.length();
    }
    res += s.substr(last_pos);

    return res;
}
enum class ResultType { RT_EOF, EXPR, STATEMENT, OTHER };
struct Result {
    ResultType type;
    std::unique_ptr<term> t;
};
inline Result parseStatement(std::istream &in) {
    std::string s;
    if (!std::getline(in, s)) {
        return {ResultType::RT_EOF, nullptr};
    }

    if (s.empty()) {
        return {ResultType::OTHER, nullptr};
    }

    std::regex assign(R"(^\s*(#[^#]+#)\s*=\s*(.*)$)");
    std::smatch match;

    if (std::regex_match(s, match, assign)) {
        std::string macro_name = match[1].str();
        std::string macro_val = match[2].str();
        replace[macro_name] = macro_val;
        return {ResultType::STATEMENT, nullptr};
    }

    std::string expanded = expand(s);
    std::stringstream ss(expanded);
    return {ResultType::EXPR, parse_term(ss)};
}
inline bool step(std::unique_ptr<term> &node) {
    if (!node)
        return false;

    auto app = dynamic_cast<application *>(node.get());
    if (app) {
        auto left_abs = dynamic_cast<abstraction *>(app->t.get());
        if (left_abs) {
            auto m_val = dynamic_cast<value *>(left_abs->t.get());

            if (m_val && m_val->variable == left_abs->parameter) {

                node = std::move(app->s);
            } else {
                left_abs->t->substitute(left_abs->parameter, app->s);

                node = std::move(left_abs->t);
            }
            return true;
        }

        if (step(app->t))
            return true;
        if (step(app->s))
            return true;

        return false;
    }

    auto abs = dynamic_cast<abstraction *>(node.get());
    if (abs) {
        return step(abs->t);
    }
    return false;
}
inline void evaluate(std::unique_ptr<term> &node, ssize_t steps = -1) {
    if (steps < 0) {
        while (step(node)) {
        };
    } else {
        for (int i = 0; i < steps; i++) {
            if (!step(node))
                break;
        }
    }
}
