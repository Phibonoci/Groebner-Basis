#pragma once

#include "concepts.h"
#include "polynomial.h"

#include <optional>

namespace GB {

Monomial Lcm(const Monomial &first, const Monomial &second) {
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

    const auto termsLCM = Lcm(l1.first, l2.first);

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
bool CheckLeadingTermsCoprime(
        const Polynomial<FieldType, MonomialOrder> &first,
        const Polynomial<FieldType, MonomialOrder> &second)
{
    auto l1 = first.GetLeadingTerm().first;
    auto l2 = second.GetLeadingTerm().first;

    return l1 * l2 == Lcm(l1, l2);
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
std::optional<Polynomial<FieldType, MonomialOrder>> CheckPair(
        const Polynomial<FieldType, MonomialOrder> &first,
        const Polynomial<FieldType, MonomialOrder> &second,
        const PolynomialSet<FieldType, MonomialOrder> &set)
{
    if (CheckLeadingTermsCoprime(first, second)) {
        return std::nullopt;
    }

    auto S = SPolynomial(first, second);

    ChainOfReductionsOverSet(S, set);
    if (S == Polynomial<FieldType, MonomialOrder>(0)) {
        return std::nullopt;
    } else {
        return S;
    }
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
PolynomialSet<FieldType, MonomialOrder> FindPairs(const PolynomialSet<FieldType, MonomialOrder> &set) {
    PolynomialSet<FieldType, MonomialOrder> suitablePairs;

    for (const auto &first : set) {
        for (const auto &second : set) {
            if (first == second) {
                break;
            }

            auto S = CheckPair(first, second, set);
            if (S.has_value()) {
                suitablePairs.insert(*S);
            }
        }
    }

    return suitablePairs;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
size_t ReductionOverSameSet(PolynomialSet<FieldType, MonomialOrder> &set) {
    size_t reductionCount = 0;

    PolynomialSet<FieldType, MonomialOrder> reducedSet;

    while (!set.empty()) {
        auto reducible = std::move(set.extract(set.begin()).value());
        reductionCount += ReductionOverSet(reducible, set);
        reductionCount += ReductionOverSet(reducible, reducedSet);

        if (reducible != Polynomial<FieldType, MonomialOrder>(0)) {
            reducedSet.insert(std::move(reducible));
        }
    }

    set = std::move(reducedSet);
    return reductionCount;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
size_t ChainOfReductionsOverSameSet(PolynomialSet<FieldType, MonomialOrder> &set) {
    size_t overallReductionCount = 0, lastReductionCount;

    while ((lastReductionCount = ReductionOverSameSet(set)) != 0) {
        overallReductionCount += lastReductionCount;
    }

    return overallReductionCount;
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
void NormalizeSetCoefficients(PolynomialSet<FieldType, MonomialOrder> &set) {
    PolynomialSet<FieldType, MonomialOrder> normalizedSet;

    for (auto &f : set) {
        FieldType normalizationCoefficient = FieldType(1) / f.GetLeadingTerm().second;
        normalizedSet.insert(f * normalizationCoefficient);
    }

    set = std::move(normalizedSet);
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
void OptimizeSet(PolynomialSet<FieldType, MonomialOrder> &set) {
    ChainOfReductionsOverSameSet(set);
    NormalizeSetCoefficients(set);
}

template<SuitableFieldType FieldType, SuitableOrder<Monomial> MonomialOrder>
void BuhbergerAlgorithm(PolynomialSet<FieldType, MonomialOrder> &set) {
    auto polynomialsToAdd = FindPairs(set);
    OptimizeSet(set);

    while (!polynomialsToAdd.empty()) {
        set.merge(polynomialsToAdd);
        polynomialsToAdd = FindPairs(set);
        OptimizeSet(set);
    }
}



} // namespace GB
