#include <cstddef>
#include <iostream>
#include <set>
#include <vector>

struct term {
    virtual std::set<size_t> FV() = 0;
};
struct value : term {
    size_t variable;
    std::set<size_t> FV() override { return {variable}; }
};
struct abstraction : term {
    size_t parameter;
    term *t;
    std::set<size_t> FV() override {
        auto fv = t->FV();
        if (fv.count(parameter))
            fv.erase(parameter);
        return fv;
    }
};
struct application : term {
    term *t, *s;
    std::set<size_t> FV() override {
        auto fv = t->FV(), fv2 = s->FV();
        for (auto i : fv2)
            fv.insert(i);
        return fv;
    }
};

int main() {}
