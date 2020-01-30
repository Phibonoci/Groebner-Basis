#pragma once

#include "rational.h"

#include <utility>
#include <vector>

namespace GB {

template<class FieldType = Rational<>>
class Monomial {
public:
    using DegreeType = OverflowDetector<uint64_t>;
    using DegreeVector = std::vector<DegreeType>;
    using IndexType = size_t;

    Monomial() = default;

    constexpr explicit Monomial(FieldType coefficient) : coefficient_(std::move(coefficient)) {
    }

    explicit Monomial(DegreeVector degrees, FieldType coefficient = 1)
        : degrees_(std::move(degrees)), coefficient_(std::move(coefficient)) {
        Shrink_();
    }

    constexpr Monomial(std::initializer_list<DegreeType> degrees, FieldType coefficient = 1)
        : degrees_(degrees), coefficient_(std::move(coefficient)) {
        Shrink_();
    }

    [[nodiscard]] IndexType GetAmountOfVariables() const noexcept {
        return degrees_.size();
    }

    [[nodiscard]] DegreeType GetDegree(const IndexType variableIndex) const noexcept {
        return variableIndex < degrees_.size() ? degrees_[variableIndex] : 0;
    }

    // TODO: Delete later if unused
    [[nodiscard]] const DegreeVector &GetDegrees() const noexcept {
        return degrees_;
    }

    const FieldType &GetCoefficient() const noexcept {
        return coefficient_;
    }

    Monomial operator+() const {
        Monomial result = *this;

        return result;
    }

    Monomial operator-() const {
        Monomial result = *this;
        result.coefficient_ *= -1;

        return result;
    }

    // TODO: Implement as polynomial private method
    Monomial &operator+=(const Monomial &other) {
        if (other.degrees_ != degrees_) {
            throw std::runtime_error("Monomials have to be the same for addition");
        }

        coefficient_ += other.coefficient_;

        Shrink_();
        return *this;
    }

    // TODO: Implement as polynomial private method
    Monomial &operator-=(const Monomial &other) {
        if (other.degrees_ != degrees_) {
            throw std::runtime_error("Monomials have to be the same for subtraction");
        }

        coefficient_ -= other.coefficient_;

        Shrink_();
        return *this;
    }

    Monomial &operator*=(const Monomial &other) {
        degrees_.resize(std::max(GetAmountOfVariables(), other.GetAmountOfVariables()));

        coefficient_ *= other.coefficient_;
        for (IndexType variableIndex = 0; variableIndex < other.GetAmountOfVariables(); ++variableIndex) {
            degrees_[variableIndex] += other.GetDegree(variableIndex);
        }

        Shrink_();
        return *this;
    }

    bool IsDivisibleBy(const Monomial &other) const {
        if (other.coefficient_ == 0 || other.GetAmountOfVariables() > GetAmountOfVariables()) {
            return false;
        }

        for (IndexType variableIndex = 0; variableIndex < other.GetAmountOfVariables(); ++variableIndex) {
            if (other.GetDegree(variableIndex) > GetDegree(variableIndex)) {
                return false;
            }
        }

        return true;
    }

    Monomial &operator/=(const Monomial &other) {
        if (other.GetAmountOfVariables() > GetAmountOfVariables()) {
            throw std::runtime_error("Monomial cannot be divided by another");
        }

        coefficient_ /= other.coefficient_;
        for (IndexType variableIndex = 0; variableIndex < other.GetAmountOfVariables(); ++variableIndex) {
            if (degrees_[variableIndex] < other.GetDegree(variableIndex)) {
                throw std::runtime_error("Monomial cannot be divided by another");
            }
            degrees_[variableIndex] -= other.GetDegree(variableIndex);
        }

        Shrink_();
        return *this;
    }

    // TODO: Implement as polynomial private method
    friend Monomial operator+(const Monomial &lhs, const Monomial &rhs) {
        Monomial result = lhs;
        result += rhs;

        return result;
    }

    // TODO: Implement as polynomial private method
    friend Monomial operator-(const Monomial &lhs, const Monomial &rhs) {
        Monomial result = lhs;
        result -= rhs;

        return result;
    }

    friend Monomial operator*(const Monomial &lhs, const Monomial &rhs) {
        Monomial result = lhs;
        result *= rhs;

        return result;
    }

    friend Monomial operator/(const Monomial &lhs, const Monomial &rhs) {
        Monomial result = lhs;
        result /= rhs;

        return result;
    }

    friend bool operator==(const Monomial &lhs, const Monomial &rhs) {
        return lhs.coefficient_ == rhs.coefficient_ && lhs.degrees_ == rhs.degrees_;
    }

    friend bool operator!=(const Monomial &lhs, const Monomial &rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream &operator<<(std::ostream &out, const Monomial &other) {
        if (bool onlyCoefficient = (other.GetAmountOfVariables() == 0); onlyCoefficient) {
            out << other.GetCoefficient();
            return out;
        }

        PrintCoefficient(out, other.GetCoefficient());

        out << "(";
        PrintVariables(out, other.degrees_);
        out << ")";

        return out;
    }

    static bool IsZero(const Monomial &other) noexcept {
        return other.coefficient_ == 0;
    }

private:
    static void PrintCoefficient(std::ostream &out, const FieldType &coefficient) {
        if (coefficient == -1) {
            out << '-';
        } else if (coefficient != 1) {
            out << coefficient;
        }
    }

    static bool PrintVariable(std::ostream &out, IndexType index, DegreeType degree) {
        if (degree == 0) {
            return false;
        }
        if (degree != 1) {
            out << "x_" << index << '^' << degree;
        } else {
            out << "x_" << index;
        }
        return true;
    }

    static void PrintVariables(std::ostream &out, const DegreeVector &degrees) {
        IndexType variableIndex = 0;
        for (; variableIndex + 1 < degrees.size(); ++variableIndex) {
            if (bool variablePrinted = PrintVariable(out, variableIndex, degrees[variableIndex]); variablePrinted) {
                out << " * ";
            }
        }
        PrintVariable(out, variableIndex, degrees[variableIndex]);
    }

    void Shrink_() {
        if (coefficient_ == FieldType(0)) {
            degrees_.resize(0);
        }

        while (degrees_.size() && degrees_.back() == 0) {
            degrees_.pop_back();
        }
    }

    DegreeVector degrees_;
    FieldType coefficient_ = FieldType(0);
};

explicit Monomial() -> Monomial<>;
explicit Monomial(int) -> Monomial<>;
explicit Monomial(std::initializer_list<DefaultIntegerType>, int) -> Monomial<>;

} // namespace GB
