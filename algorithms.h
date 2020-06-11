#pragma once

#include "concepts.h"
#include "polynomial.h"

#include <optional>

namespace GB {

Monomial lcm(const Monomial &first, const Monomial &second) {
    Monomial::DegreeVector result(std::max(first.GetAmountOfVariables(), second.GetAmountOfVariables()));
    for (Monomial::IndexType index = 0; index < result.size(); ++index) {
        result[index] = std::max(first.GetDegree(index), second.GetDegree(index));
    }
    return Monomial(result);
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
Polynomial<FieldType, MonomialOrder> SPolynomial (
        const Polynomial<FieldType, MonomialOrder> &first,
        const Polynomial<FieldType, MonomialOrder> &second)
{
    const auto &l1 = first.GetLeadingTerm();
    const auto &l2 = second.GetLeadingTerm();

    const auto termsLCM = lcm(l1.first, l2.first);

    return termsLCM / l1.first * first * l2.second - termsLCM / l2.first * second * l1.second;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
bool ElementaryReduction(
        Polynomial<FieldType, MonomialOrder> &reducible,
        const Polynomial<FieldType, MonomialOrder> &other)
{
    const auto &termToDivide = std::find_if(reducible.begin(), reducible.end(), [&] (const auto &term) {
        return term.first.IsDivisibleBy(other.GetLeadingTerm().first);
    });

    if (termToDivide == reducible.end()) {
        return false;
    }

    typename Polynomial<FieldType, MonomialOrder>::Term quotient = {
            termToDivide->first / other.GetLeadingTerm().first,
            termToDivide->second / other.GetLeadingTerm().second
    };

    reducible -= Polynomial(quotient) * other;

    return true;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
size_t ChainOfElementaryReductions(
        Polynomial<FieldType, MonomialOrder> &reducible,
        const Polynomial<FieldType, MonomialOrder> &other)
{
    size_t reductionCount = 0;

    while (ElementaryReduction(reducible, other)) {
        ++reductionCount;
    }

    return reductionCount;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
size_t ReductionOverSet(
        Polynomial<FieldType, MonomialOrder> &reducible,
        const PolynomialSet<FieldType, MonomialOrder> &other)
{
    size_t reductionCount = 0;
    for (const auto &f : other) {
        reductionCount += ChainOfElementaryReductions(reducible, f);
    }

    return reductionCount;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
size_t ChainOfReductionsOverSet(
        Polynomial<FieldType, MonomialOrder> &reducible,
        const PolynomialSet<FieldType, MonomialOrder> &other)
{
    size_t overallReductionCount = 0, lastReductionCount;
    while ((lastReductionCount = ReductionOverSet(reducible, other)) != 0) {
        overallReductionCount += lastReductionCount;
    }

    return overallReductionCount;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
std::optional<Polynomial<FieldType, MonomialOrder>> CheckPair(
        const Polynomial<FieldType, MonomialOrder> &first,
        const Polynomial<FieldType, MonomialOrder> &second,
        const PolynomialSet<FieldType, MonomialOrder> &set)
{
    auto S = SPolynomial(first, second);

    ChainOfReductionsOverSet(S, set);
    if (S == Polynomial<FieldType, MonomialOrder>(0)) {
        return std::nullopt;
    } else {
        return S;
    }
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
std::optional<Polynomial<FieldType, MonomialOrder>> FindPair(const PolynomialSet<FieldType, MonomialOrder> &set) {
    for (const auto &first : set) {
        for (const auto &second : set) {
            if (first == second) {
                continue;
            }

            if (auto S = CheckPair(first, second, set); S.has_value()) {
                return S;
            }
        }
    }

    return std::nullopt;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
void BuhbergerAlgorithm(PolynomialSet<FieldType, MonomialOrder> &set) {
    auto polynomialToAdd = FindPair(set);

    while (polynomialToAdd.has_value()) {
        set.insert(*polynomialToAdd);
        polynomialToAdd = FindPair(set);
    }
}



} // namespace GB