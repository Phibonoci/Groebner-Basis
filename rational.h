#pragma once

#include <cstdint>
#include <type_traits>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

namespace GB {

template<class IntegerType = int64_t, class = typename std::enable_if_t<std::is_integral_v<IntegerType>>>
class Rational {
// The following class invariants are used:
// numerator is an integer,
// denominator is a positive integer,
// numerator and denominator are co-prime.

public:
    Rational() : numerator_(0), denominator_(1) {
    }

    Rational(IntegerType numerator) : numerator_(numerator), denominator_(1) {
    }

    Rational(IntegerType numerator, IntegerType denominator) : numerator_(numerator), denominator_(denominator) {
        if (denominator_ == 0) {
            throw std::overflow_error("Divide by zero exception");
        }
        _Reduce();
        _CheckInvariants();
    }

    IntegerType GetNumerator() const {
        return numerator_;
    }

    IntegerType GetDenominator() const {
        return denominator_;
    }

    void Invert() {
        if (numerator_ == 0) {
            throw std::overflow_error("Divide by zero exception");
        }
        std::swap(numerator_, denominator_);
        _Reduce();
        _CheckInvariants();
    }

    Rational GetInverted() const {
        Rational result = *this;

        result.Invert();
        return result;
    }

    explicit operator double() const {
        if (denominator_ == 0) {
            throw std::overflow_error("Divide by zero exception");
        }
        return static_cast<double>(numerator_) / denominator_;
    }

    Rational operator+() const {
        Rational result = *this;

        result._CheckInvariants();
        return result;
    }

    Rational operator-() const {
        Rational result = *this;
        result.numerator_ *= -1;

        result._CheckInvariants();
        return result;
    }

    Rational &operator+=(const Rational &other) {
        assert(denominator_ != 0 && other.denominator_ != 0);

        int64_t denominators_lcm = std::lcm(denominator_, other.denominator_);
        numerator_ *= denominators_lcm / denominator_;
        denominator_ = denominators_lcm;
        numerator_ += denominators_lcm / other.denominator_ * other.numerator_;

        _Reduce();
        _CheckInvariants();
        return *this;
    }

    Rational &operator-=(const Rational &other) {
        assert(denominator_ != 0 && other.denominator_ != 0);

        int64_t denominators_lcm = std::lcm(denominator_, other.denominator_);
        numerator_ *= denominators_lcm / denominator_;
        denominator_ = denominators_lcm;
        numerator_ -= denominators_lcm / other.denominator_ * other.numerator_;

        _Reduce();
        _CheckInvariants();
        return *this;
    }

    Rational &operator*=(const Rational &other) {
        numerator_ *= other.numerator_;
        denominator_ *= other.denominator_;

        _Reduce();
        _CheckInvariants();
        return *this;
    }

    Rational &operator/=(const Rational &other) {
        numerator_ *= other.denominator_;
        denominator_ *= other.numerator_;

        _Reduce();
        _CheckInvariants();
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
        lhs._CheckInvariants();
        rhs._CheckInvariants();

        return lhs.numerator_ == rhs.numerator_ && lhs.denominator_ == rhs.denominator_;
    }

    friend bool operator!=(const Rational &lhs, const Rational &rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(const Rational &lhs, const Rational &rhs) {
        lhs._CheckInvariants();
        rhs._CheckInvariants();

        int64_t denominators_lcm = std::lcm(lhs.denominator_, rhs.denominator_);
        return (lhs.numerator_ * (denominators_lcm / lhs.denominator_)) <
               (rhs.numerator_ * (denominators_lcm / rhs.denominator_));
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
        out << other.numerator_ << '/' << other.denominator_;
        return out;
    }

private:
    // This function keeps class invariants correct.
    void _Reduce() {
        if (denominator_ < 0) {
            denominator_ *= -1;
            numerator_ *= -1;
        }

        IntegerType gcd = std::gcd(numerator_, denominator_);

        numerator_ /= gcd;
        denominator_ /= gcd;
    }

    // This function checks class invariants.
    void _CheckInvariants() const {
        assert(denominator_ > 0);
        assert(std::gcd(numerator_, denominator_) == 1);
    }

    IntegerType numerator_, denominator_;
};

} // namespace GB
