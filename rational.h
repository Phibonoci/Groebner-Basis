#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <type_traits>

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
    }

    Rational(const Rational &other) = default;

    ~Rational() = default;

    IntegerType GetNumerator() const {
        _CheckInvariant();
        return numerator_;
    }

    IntegerType GetDenominator() const {
        _CheckInvariant();
        return denominator_;
    }

    void Invert() {
        if (numerator_ == 0) {
            throw std::overflow_error("Divide by zero exception");
        }
        std::swap(numerator_, denominator_);
        _Reduce();
    }

    Rational GetInverted() const {
        Rational temp = *this;

        temp.Invert();

        temp._Reduce();
        return temp;
    }

    double ToDouble() const {
        _CheckInvariant();
        return static_cast<double>(numerator_) / denominator_;
    }

    Rational operator+() {
        Rational temp = *this;

        temp._CheckInvariant();
        return temp;
    }

    Rational operator-() {
        Rational temp = *this;
        temp.numerator_ *= -1;

        _CheckInvariant();
        return temp;
    }

    Rational &operator=(const Rational &other) {
        numerator_ = other.numerator_;
        denominator_ = other.denominator_;

        _CheckInvariant();
        return *this;
    }

    Rational &operator+=(const Rational &other) {
        int64_t temp = std::lcm(denominator_, other.denominator_);
        numerator_ *= temp / denominator_;
        denominator_ = temp;
        numerator_ += temp / other.denominator_ * other.numerator_;

        _Reduce();
        return *this;
    }

    Rational &operator-=(const Rational &other) {
        int64_t temp = std::lcm(denominator_, other.denominator_);
        numerator_ *= temp / denominator_;
        denominator_ = temp;
        numerator_ -= temp / other.denominator_ * other.numerator_;

        _Reduce();
        return *this;
    }

    Rational &operator*=(const Rational &other) {
        numerator_ *= other.numerator_;
        denominator_ *= other.denominator_;

        _Reduce();
        return *this;
    }

    Rational &operator/=(const Rational &other) {
        numerator_ *= other.denominator_;
        denominator_ *= other.numerator_;

        _Reduce();
        return *this;
    }

    friend inline Rational operator+(const Rational &lhs, const Rational &rhs) {
        Rational result = lhs;

        result += rhs;

        return result;
    }

    friend inline Rational operator-(const Rational &lhs, const Rational &rhs) {
        Rational result = lhs;

        result -= rhs;

        return result;
    }

    friend inline Rational operator*(const Rational &lhs, const Rational &rhs) {
        Rational temp = lhs;

        temp *= rhs;

        return temp;
    }

    friend inline Rational operator/(const Rational &lhs, const Rational &rhs) {
        Rational result = lhs;

        result /= rhs;

        return result;
    }

    friend inline bool operator==(const Rational &lhs, const Rational &rhs) {
        return lhs.numerator_ == rhs.numerator_ && lhs.denominator_ == rhs.denominator_;
    }

    friend inline bool operator!=(const Rational &lhs, const Rational &rhs) {
        return !(lhs == rhs);
    }

    friend inline bool operator<(const Rational &lhs, const Rational &rhs) {
        int64_t temp = std::lcm(lhs.denominator_, rhs.denominator_);
        return (lhs.numerator_ * (temp / lhs.denominator_)) < (rhs.numerator_ * (temp / rhs.denominator_));
    }

    friend inline bool operator>(const Rational &lhs, const Rational &rhs) {
        return !((lhs == rhs) || lhs < rhs);
    }

    friend inline bool operator<=(const Rational &lhs, const Rational &rhs) {
        return lhs < rhs || lhs == rhs;
    }

    friend inline bool operator>=(const Rational &lhs, const Rational &rhs) {
        return !(lhs < rhs);
    }
    
    friend std::ostream &operator<<(std::ostream &out, const Rational &other) {
        out << other.numerator_ << '/' << other.denominator_;
        return out;
    }

private:
    IntegerType numerator_, denominator_;

    // This function keeps class invariants correct.
    void _Reduce() {
        if (denominator_ < 0) {
            denominator_ *= -1;
            numerator_ *= -1;
        }

        IntegerType temp = std::gcd(numerator_, denominator_);

        numerator_ /= temp;
        denominator_ /= temp;
    }


    // This function checks class invariants.
    void _CheckInvariant() {
        assert(denominator_ > 0);
        assert(numerator_ % denominator_ != 0);
    }
};

} // GB namespace
