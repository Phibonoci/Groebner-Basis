#pragma once

#include "rational.h"

#include <utility>
#include <vector>

namespace GB {

template<class FieldType = Rational<>>
class Monomial {
public:
    using DegreeType = DefaultOverflowDetectedType;
    using DegreeVector = std::vector<DegreeType>;

    Monomial() : coefficient_(1) {
    }

    explicit Monomial(FieldType coefficient) : degrees_(DegreeVector()), coefficient_(coefficient) {
    }

    explicit Monomial(DegreeVector degrees, FieldType coefficient = 1)
        : degrees_(std::move(degrees)), coefficient_(coefficient) {
        Shrink_();
    }

    explicit Monomial(DegreeVector &&degrees, FieldType coefficient = 1)
        : degrees_(std::move(degrees)), coefficient_(coefficient) {
        Shrink_();
    }

    Monomial(std::initializer_list<DegreeType> degrees, FieldType coefficient = 1)
        : degrees_(degrees), coefficient_(coefficient) {
        Shrink_();
    }

    [[nodiscard]] size_t GetMaxVariableIndex() const noexcept {
        return degrees_.size();
    }

    [[nodiscard]] DegreeType GetDegree(const size_t variableIndex) const noexcept {
        return variableIndex < degrees_.size() ? degrees_[variableIndex] : 0;
    }

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

    Monomial &operator*=(const Monomial &other) {
        degrees_.resize(std::max(GetMaxVariableIndex(), other.GetMaxVariableIndex()));

        coefficient_ *= other.coefficient_;
        for (size_t variableIndex = 0; variableIndex < other.GetMaxVariableIndex(); ++variableIndex) {
            degrees_[variableIndex] += other.GetDegree(variableIndex);
        }

        Shrink_();
        return *this;
    }

    bool IsDivisibleBy(const Monomial& other) const {
        if (other.coefficient_ == 0) {
            return false;
        }

        size_t maxCommonVariableIndex = std::min(GetMaxVariableIndex(), other.GetMaxVariableIndex());

        for (size_t variableIndex = 0; variableIndex < maxCommonVariableIndex; ++variableIndex) {
            if (other.GetDegree(variableIndex) > GetDegree(variableIndex)) {
                return false;
            }
        }

        return true;
    }

    Monomial &operator/=(const Monomial &other) {
        degrees_.resize(std::max(GetMaxVariableIndex(), other.GetMaxVariableIndex()));

        coefficient_ /= other.coefficient_;
        for (size_t variableIndex = 0; variableIndex < other.GetMaxVariableIndex(); ++variableIndex) {
            degrees_[variableIndex] -= other.GetDegree(variableIndex);
            if (degrees_[variableIndex] < 0) {
                throw std::runtime_error("Monomial cannot be divided by another");
            }
        }

        Shrink_();
        return *this;
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
        bool multiplicationSignFlag = false, onlyCoefficient = (other.GetMaxVariableIndex() == 0);

        if (onlyCoefficient || abs(other.GetCoefficient()) != 1) {
            std::cout << other.GetCoefficient();
        } else {
            if (other.GetCoefficient() == -1) {
                std::cout << '-';
            }
        }

        if (!onlyCoefficient) {
            std::cout << "(";
        }
        for (size_t variableIndex = 0; variableIndex < other.GetMaxVariableIndex(); ++variableIndex) {
            if (DegreeType degree = other.GetDegree(variableIndex); degree != 0) {
                if (multiplicationSignFlag) {
                    std::cout << " * ";
                }
                if (degree != 1) {
                    std::cout << "x_" << variableIndex << '^' << degree;
                } else {
                    std::cout << "x_" << variableIndex;
                }
                multiplicationSignFlag = true;
            }
        }
        if (!onlyCoefficient) {
            std::cout << ")";
        }

        return out;
    }

    static bool IsZero(const Monomial &other) noexcept {
        return other.coefficient_ == 0;
    }

private:

    void Shrink_() {
        while (degrees_.size() && degrees_.back() == 0) {
            degrees_.pop_back();
        }
    }

    DegreeVector degrees_;
    FieldType coefficient_;
};

} // namespace GB
