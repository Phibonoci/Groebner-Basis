#pragma once

#include "rational.h"

#include <utility>
#include <vector>
#include <algorithm>

namespace GB {

class Monomial {
public:
    using DegreeType = OverflowDetector<uint64_t>;
    using DegreeVector = std::vector<DegreeType>;
    using IndexType = size_t;

    Monomial() = default;

    Monomial(std::initializer_list<DegreeType> degrees) : degrees_(degrees) {
        Shrink_();
    }

    explicit Monomial(DegreeVector degrees) : degrees_(std::move(degrees)) {
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

    [[nodiscard]] DegreeType TotalDegree() const noexcept {
        return std::accumulate(degrees_.begin(), degrees_.end(), DegreeType());
    }

    Monomial &operator*=(const Monomial &other) {
        degrees_.resize(std::max(GetAmountOfVariables(), other.GetAmountOfVariables()));

        for (IndexType variableIndex = 0; variableIndex < other.GetAmountOfVariables(); ++variableIndex) {
            degrees_[variableIndex] += other.GetDegree(variableIndex);
        }

        Shrink_();
        return *this;
    }

    [[nodiscard]] bool IsDivisibleBy(const Monomial &other) const {
        if (other.GetAmountOfVariables() > GetAmountOfVariables()) {
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

        for (IndexType variableIndex = 0; variableIndex < other.GetAmountOfVariables(); ++variableIndex) {
            if (degrees_[variableIndex] < other.GetDegree(variableIndex)) {
                throw std::runtime_error("Monomial cannot be divided by another");
            }
            degrees_[variableIndex] -= other.GetDegree(variableIndex);
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

    friend bool operator<(const Monomial &lhs, const Monomial &rhs) {
        return lhs.degrees_ < rhs.degrees_;
    }

    friend bool operator==(const Monomial &lhs, const Monomial &rhs) {
        return lhs.degrees_ == rhs.degrees_;
    }

    friend bool operator!=(const Monomial &lhs, const Monomial &rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream &operator<<(std::ostream &out, const Monomial &other) {

        if (HasNoVariables(other)) {
            out << "0";
            return out;
        }

        out << "(";
        PrintVariables_(out, other.degrees_);
        out << ")";

        return out;
    }

    static bool HasNoVariables(const Monomial &other) noexcept {
        return other.degrees_.empty();
    }

private:
    static bool PrintVariable_(std::ostream &out, IndexType index, DegreeType degree) {
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

    static void PrintVariables_(std::ostream &out, const DegreeVector &degrees) {
        IndexType variableIndex = 0;
        for (; variableIndex + 1 < degrees.size(); ++variableIndex) {
            if (bool isVariablePrinted = PrintVariable_(out, variableIndex, degrees[variableIndex]); isVariablePrinted) {
                out << " * ";
            }
        }
        PrintVariable_(out, variableIndex, degrees[variableIndex]);
    }

    void Shrink_() {
        while (!degrees_.empty() && degrees_.back() == 0) {
            degrees_.pop_back();
        }
    }

    DegreeVector degrees_;
};

} // namespace GB
