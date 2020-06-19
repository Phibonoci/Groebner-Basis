#pragma once

#include "polynomial.h"

namespace GB {



template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
void BuildCyclic(size_t index, size_t remain, size_t upperBound, Monomial &m, Polynomial<FieldType, MonomialOrder> &f) {
    if (remain == 0) {
        f += m;
        return;
    }

    if (index == upperBound) {
        return;
    }

    m.SetVariableDegree(index, 1);
    BuildCyclic(index + 1, remain - 1, upperBound, m, f);
    m.SetVariableDegree(index, 0);
    BuildCyclic(index + 1, remain, upperBound, m, f);
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
Polynomial<FieldType, MonomialOrder> BuildElementaryCyclePolymonial(size_t n, size_t m) {
    Polynomial<FieldType, MonomialOrder> f;
    Monomial temp;
    BuildCyclic(0, n, m, temp, f);
    return f;
}

template<SuitableFieldType FieldType = Rational<>, SuitableOrder<Monomial> MonomialOrder = LexicographicalOrder>
PolynomialSet<FieldType, MonomialOrder> BuildCycleSet(size_t m) {
    PolynomialSet<FieldType, MonomialOrder> ans;
    for (size_t n = 1; n <= m; ++n) {
        auto temp = BuildElementaryCyclePolymonial<FieldType, MonomialOrder>(n, m);
        if (n == m) {
            temp += m % 2 == 1 ? FieldType(-1) : FieldType(1);
        }
        ans.insert(std::move(temp));
    }

    return ans;
}


} // namespace GB