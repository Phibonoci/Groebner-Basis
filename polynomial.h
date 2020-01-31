#pragma once

#include "rational.h"
#include "monomial.h"

#include <map>

namespace GB {

template<class FieldType = Rational<>>
class Polynomial {
public:
    using TermMap = std::map<Monomial, FieldType>;
    using Term = typename TermMap::value_type;
    using IndexType = size_t;

    Polynomial() = default;

    Polynomial(std::initializer_list<Term> terms) : terms_(std::move(terms)) {
        Shrink_();
    }

    explicit Polynomial(Monomial monomial) : terms_{{std::move(monomial), 1}} {
    }

    explicit constexpr Polynomial(Term term) {
        if (term.second != 0) {
            terms_.insert(std::move(term));
        }
    }

    explicit Polynomial(FieldType coefficient) : terms_{{{}, std::move(coefficient)}} {
        Shrink_();
    }

    [[nodiscard]] IndexType GetAmountOfTerms() const noexcept {
        return terms_.size();
    }

    Term GetNthTerm(IndexType index) const {
        assert(index < terms_.size());
        return *std::next(begin(), index);
    }

    Polynomial operator+() const {
        Polynomial result = *this;

        return result;
    }

    Polynomial operator-() const {
        Polynomial result = *this;
        for (auto &term : result) {
            term.second *= -1;
        }

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

    typename TermMap::iterator rbegin() noexcept {
        return terms_.begin();
    }

    typename TermMap::const_iterator rbegin() const noexcept {
        return terms_.cbegin();
    }

    typename TermMap::reverse_iterator end() noexcept {
        return terms_.rend();
    }

    typename TermMap::const_reverse_iterator end() const noexcept {
        return terms_.crend();
    }

    typename TermMap::iterator rend() noexcept {
        return terms_.end();
    }

    typename TermMap::const_iterator rend() const noexcept {
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

    static bool IsEmpty(const Polynomial &other) {
        return other.terms_.empty();
    }

    friend std::ostream &operator<<(std::ostream &out, const Polynomial &other) {
        if (IsEmpty(other)) {
            out << "0";
            return out;
        }

        for (auto iter = other.begin(); iter != std::prev(other.end());) {
            PrintTerm(out, *iter++);
            out << (iter->second < 0 ? " - " : " + ");
        }

        PrintTerm(out, *other.rbegin());

        return out;
    }

private:
    static void PrintTerm(std::ostream &out, const Term &term) {
        if (Monomial::HasNoVariables(term.first)) {
            out << term.second;
            return;
        }

        if (auto absCoefficient = abs(term.second); absCoefficient != 1) {
            out << absCoefficient;
        }

        out << term.first;
    }

    void AddTerm_(const Term &term) {
        if (auto foundTerm = terms_.lower_bound(term.first); foundTerm != terms_.end() && *foundTerm == term) {
            foundTerm->second += term.second;
            if (foundTerm->second == 0) {
                terms_.erase(foundTerm);
            }
        } else {
            terms_.insert(foundTerm, term);
        }
    }

    void SubtractTerm_(const Term &term) {
        if (auto foundTerm = terms_.lower_bound(term.first); foundTerm != terms_.end() && *foundTerm == term) {
            foundTerm->second -= term.second;
            if (foundTerm->second == 0) {
                terms_.erase(foundTerm);
            }
        } else {
            terms_.insert(foundTerm, {term.first, term.second});
        }
    }

    void CheckInvariants_() const noexcept {
        assert(std::none_of(terms_.begin(), terms_.end(), [] (const Term &term) {
            return term.second == 0;
        }));
    }

    void Shrink_() {
        for (auto constIter = terms_.begin(); constIter != terms_.end();) {
            if (constIter->second == 0) {
                constIter = terms_.erase(constIter);
            } else {
                ++constIter;
            }
        }
    }

    TermMap terms_;
};

explicit Polynomial() -> Polynomial<>;
explicit Polynomial(int) -> Polynomial<>;
explicit Polynomial(std::pair<Monomial, int>) -> Polynomial<>;
explicit Polynomial(std::initializer_list<std::pair<Monomial, int>>) -> Polynomial<>;

}
