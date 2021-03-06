#pragma once

#include "concepts.h"
#include "overflow_detector.h"

#include <cstdint>
#include <type_traits>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

namespace GB {

using DefaultIntegerType = int64_t;

template<Integral IntegerType = DefaultIntegerType>
class Rational {
// The following class invariants are used:
// numerator is an integer,
// denominator is a positive integer,
// numerator and denominator are co-prime.

public:
    using OverflowDetectedIntegerType = OverflowDetector<IntegerType>;

    Rational() noexcept : numerator_(0), denominator_(1) {
    }

    Rational(IntegerType numerator) noexcept : numerator_(numerator), denominator_(1) {
    }

    Rational(IntegerType numerator, IntegerType denominator) : numerator_(numerator), denominator_(denominator) {
        if (denominator_ == OverflowDetectedIntegerType(0)) {
            throw std::overflow_error("Divide by zero exception");
        }
        Reduce_();
        CheckInvariants_();
    }

    [[nodiscard]] IntegerType GetNumerator() const noexcept {
        return numerator_;
    }

    [[nodiscard]] IntegerType GetDenominator() const noexcept {
        return denominator_;
    }

    void Invert() {
        if (numerator_ == OverflowDetectedIntegerType(0)) {
            throw std::overflow_error("Divide by zero exception");
        }
        std::swap(numerator_, denominator_);
        Reduce_();
        CheckInvariants_();
    }

    Rational GetInverted() const {
        Rational result = *this;

        result.Invert();
        return result;
    }

    explicit operator double() const {
        assert(denominator_ != 0);
        return static_cast<double>(numerator_) / denominator_;
    }

    Rational operator+() const {
        Rational result = *this;

        return result;
    }

    Rational operator-() const {
        Rational result = *this;
        result.numerator_ *= -1;

        return result;
    }

    Rational &operator+=(const Rational &other) {
        assert(denominator_ != OverflowDetectedIntegerType(0) && other.denominator_ != OverflowDetectedIntegerType(0));

        auto denominators_lcm = OverflowDetectedIntegerType::lcm(denominator_, other.denominator_);
        numerator_ *= denominators_lcm / denominator_;
        denominator_ = denominators_lcm;
        numerator_ += denominators_lcm / other.denominator_ * other.numerator_;

        Reduce_();
        CheckInvariants_();
        return *this;
    }

    Rational &operator-=(const Rational &other) {
        assert(denominator_ != OverflowDetectedIntegerType(0) && other.denominator_ != OverflowDetectedIntegerType(0));

        auto denominators_lcm = OverflowDetectedIntegerType::lcm(denominator_, other.denominator_);
        numerator_ *= denominators_lcm / denominator_;
        denominator_ = denominators_lcm;
        numerator_ -= denominators_lcm / other.denominator_ * other.numerator_;

        Reduce_();
        CheckInvariants_();
        return *this;
    }

    Rational &operator*=(const Rational &other) {
        numerator_ *= other.numerator_;
        denominator_ *= other.denominator_;

        Reduce_();
        CheckInvariants_();
        return *this;
    }

    Rational &operator/=(const Rational &other) {
        if (other.numerator_ == OverflowDetectedIntegerType(0)) {
            throw std::overflow_error("Divide by zero exception");
        }

        numerator_ *= other.denominator_;
        denominator_ *= other.numerator_;

        Reduce_();
        CheckInvariants_();
        return *this;
    }

    // There is no need to check invariants, due to inplace operators check them.

    friend Rational operator+(const Rational &lhs, const Rational &rhs) {
        Rational result = lhs;
        result += rhs;

        return result;
    }

    friend Rational operator-(const Rational &lhs, const Rational &rhs) {
        Rational result = lhs;
        result -= rhs;

        return result;
    }

    friend Rational operator*(const Rational &lhs, const Rational &rhs) {
        Rational result = lhs;
        result *= rhs;

        return result;
    }

    friend Rational operator/(const Rational &lhs, const Rational &rhs) {
        Rational result = lhs;
        result /= rhs;

        return result;
    }

    // Check invariants only in == and < operators, because all other
    // overloaded operators are expressed in terms of these two.
    // We need to check both invariants, because (-5 / 5 == 1 / -1)

    friend bool operator==(const Rational &lhs, const Rational &rhs) {
        lhs.CheckInvariants_();
        rhs.CheckInvariants_();

        return lhs.numerator_ == rhs.numerator_ && lhs.denominator_ == rhs.denominator_;
    }

    friend bool operator!=(const Rational &lhs, const Rational &rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(const Rational &lhs, const Rational &rhs) {
        lhs.CheckInvariants_();
        rhs.CheckInvariants_();

        auto denominators_gcd = OverflowDetectedIntegerType::gcd(lhs.denominator_, rhs.denominator_);
        return (lhs.numerator_ * (rhs.denominator_ / denominators_gcd)) <
               (rhs.numerator_ * (lhs.denominator_ / denominators_gcd));
    }

    friend bool operator>(const Rational &lhs, const Rational &rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(const Rational &lhs, const Rational &rhs) {
        return !(rhs < lhs);
    }

    friend bool operator>=(const Rational &lhs, const Rational &rhs) {
        return !(lhs < rhs);
    }

    friend std::ostream &operator<<(std::ostream &out, const Rational &other) {
        if (other.denominator_ == 1) {
            out << other.numerator_;
        } else {
            out << other.numerator_ << '/' << other.denominator_;
        }

        return out;
    }

private:
    // This function keeps class invariants correct.
    void Reduce_() {
        if (denominator_ < OverflowDetectedIntegerType(0)) {
            denominator_ *= -1;
            numerator_ *= -1;
        }

        auto gcd = OverflowDetectedIntegerType::gcd(numerator_, denominator_);

        numerator_ /= gcd;
        denominator_ /= gcd;
    }

    // This function checks class invariants.
    void CheckInvariants_() const noexcept {
        assert(denominator_ > OverflowDetectedIntegerType(0));
        assert(OverflowDetectedIntegerType::gcd(numerator_, denominator_) == OverflowDetectedIntegerType(1));
    }

    OverflowDetector<IntegerType> numerator_, denominator_;
};

template<Integral T>
Rational<T> abs(const Rational<T> &other) {
    return other < 0 ? -other : other;
}

} // namespace GB
