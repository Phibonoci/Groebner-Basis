#pragma once

#include "concepts.h"
#include "rational.h"
#include "monomial.h"
#include "order.h"

#include <set>
#include <map>

namespace GB {

template<SuitableFieldType FieldType = Rational<>, SuitableOrder<Monomial> MonomialOrder = LexicographicalOrder>
class Polynomial {
public:
    using TermMap = std::map<Monomial, FieldType, MonomialOrder>;
    using Term = typename TermMap::value_type;
    using IndexType = Monomial::IndexType;

    Polynomial() = default;

    Polynomial(std::initializer_list<Term> terms) : terms_(terms) {
        Shrink_();
    }

    Polynomial(Monomial monomial) : terms_{{std::move(monomial), 1}} {
    }

    explicit constexpr Polynomial(Term term) : terms_{std::move(term)} {
        Shrink_();
    }

    Polynomial(FieldType coefficient) : terms_{{{}, std::move(coefficient)}} {
        Shrink_();
    }

    template<SuitableOrder<Monomial> OtherMonomialOrder>
    Polynomial(const Polynomial<FieldType, OtherMonomialOrder> &other) {
        for (const auto &term : other) {
            terms_.insert(term);
        }
    }

    [[nodiscard]] IndexType GetAmountOfTerms() const noexcept {
        return terms_.size();
    }

    Term GetNthTerm(IndexType index) const {
        assert(index < terms_.size());
        return *std::next(begin(), index);
    }

    Term GetLeadingTerm() const {
        return *begin();
    }

    Polynomial operator+() const {
        Polynomial result = *this;

        return result;
    }

    Polynomial operator-() const {
        Polynomial result = *this;
        result *= Polynomial(-1);

        return result;
    }

    Polynomial &operator+=(const Polynomial &other) {
        for (const auto &term : other) {
            AddTerm_(term);
        }

        CheckInvariants_();
        return *this;
    }

    friend Polynomial operator+(const Polynomial &lhs, const Polynomial &rhs) {
        Polynomial result = lhs;
        result += rhs;

        return result;
    }

    Polynomial &operator-=(const Polynomial &other) {
        for (const auto &term : other) {
            SubtractTerm_(term);
        }

        CheckInvariants_();
        return *this;
    }

    friend Polynomial operator-(const Polynomial &lhs, const Polynomial &rhs) {
        Polynomial result = lhs;
        result -= rhs;

        return result;
    }

    Polynomial &operator*=(const Polynomial &other) {
        *this = *this * other;
        return *this;
    }

    friend Polynomial operator*(const Polynomial &lhs, const Polynomial &rhs) {
        Polynomial result;
        for (const auto &leftTerm : lhs) {
            for (const auto &rightTerm : rhs) {
                result.AddTerm_(Term{leftTerm.first * rightTerm.first, leftTerm.second * rightTerm.second});
            }
        }

        result.CheckInvariants_();
        return result;
    }

    typename TermMap::reverse_iterator begin() noexcept {
        return terms_.rbegin();
    }

    typename TermMap::const_reverse_iterator begin() const noexcept {
        return terms_.crbegin();
    }

    typename TermMap::const_reverse_iterator cbegin() const noexcept {
        return terms_.crbegin();
    }

    typename TermMap::iterator rbegin() noexcept {
        return terms_.begin();
    }

    typename TermMap::const_iterator rbegin() const noexcept {
        return terms_.cbegin();
    }

    typename TermMap::const_iterator crbegin() const noexcept {
        return terms_.cbegin();
    }

    typename TermMap::reverse_iterator end() noexcept {
        return terms_.rend();
    }

    typename TermMap::const_reverse_iterator end() const noexcept {
        return terms_.crend();
    }

    typename TermMap::const_reverse_iterator cend() const noexcept {
        return terms_.crend();
    }

    typename TermMap::iterator rend() noexcept {
        return terms_.end();
    }

    typename TermMap::const_iterator rend() const noexcept {
        return terms_.cend();
    }

    typename TermMap::const_iterator crend() const noexcept {
        return terms_.cend();
    }

    friend bool operator==(const Polynomial &lhs, const Polynomial &rhs) {
        lhs.CheckInvariants_();
        rhs.CheckInvariants_();

        return lhs.terms_ == rhs.terms_;
    }

    friend bool operator!=(const Polynomial &lhs, const Polynomial &rhs) {
        return !(lhs == rhs);
    }

    static bool IsZero(const Polynomial &other) {
        return other.terms_.empty();
    }

    friend std::ostream &operator<<(std::ostream &out, const Polynomial &other) {
        if (IsZero(other)) {
            out << "0";
            return out;
        }

        if (other.begin()->second < 0) {
            out << "-";
        }

        for (auto iter = other.begin(); iter != std::prev(other.end());) {
            PrintTerm(out, *iter++);
            out << (iter->second < 0 ? " - " : " + ");
        }

        PrintTerm(out, *other.rbegin());

        return out;
    }

    static Monomial &GetMonomial(const Term &term) {
        return term.first;
    }

    static FieldType &GetCoefficient(const Term &term) {
        return term.second;
    }

private:
    static void PrintTerm(std::ostream &out, const Term &term) {
        auto absCoefficient = abs(term.second);
        if (absCoefficient != 1) {
            out << absCoefficient;
        }

        if (!Monomial::HasNoVariables(term.first)) {
            out << term.first;
        } else {
            out << absCoefficient;
        }
    }

    void AddTerm_(const Term &term) {
        auto foundTerm = terms_.lower_bound(term.first);

        if (foundTerm != terms_.end() && foundTerm->first == term.first) {
            foundTerm->second += term.second;
            if (foundTerm->second == 0) {
                terms_.erase(foundTerm);
            }
        } else {
            terms_.insert(foundTerm, term);
        }
    }

    void SubtractTerm_(const Term &term) {
        auto foundTerm = terms_.lower_bound(term.first);

        if (foundTerm != terms_.end() && foundTerm->first == term.first) {
            foundTerm->second -= term.second;

            if (foundTerm->second == 0) {
                terms_.erase(foundTerm);
            }
        } else {
            terms_.insert(foundTerm, {term.first, -term.second});
        }
    }

    void CheckInvariants_() const noexcept {
        assert(std::none_of(terms_.begin(), terms_.end(), [] (const Term &term) {
            return term.second == 0;
        }));
    }

    void Shrink_() {
        for (auto iter = terms_.begin(); iter != terms_.end();) {
            if (iter->second == 0) {
                iter = terms_.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    TermMap terms_;

    template<SuitableFieldType>
    friend struct Less;
};

explicit Polynomial() -> Polynomial<>;
explicit Polynomial(int) -> Polynomial<>;
explicit Polynomial(std::pair<Monomial, int>) -> Polynomial<>;
explicit Polynomial(std::initializer_list<std::pair<Monomial, int>>) -> Polynomial<>;

// TODO: Rewrite using custom hash function with efficient changes. Therefore change set to unordered_set.
template<SuitableFieldType FieldType = Rational<>>
struct Less {
    bool operator()(const Polynomial<FieldType> &lhs, const Polynomial<FieldType> &rhs) const noexcept {
        return lhs.terms_ < rhs.terms_;
    }
};

template<SuitableFieldType FieldType = Rational<>, SuitableOrder<Monomial> MonomialOrder = LexicographicalOrder>
using PolynomialSet = std::set<Polynomial<FieldType, MonomialOrder>, Less<FieldType>>;

} // namespace GB
