#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <type_traits>

namespace GB {

template<class IntegerType = int64_t, class = typename std::enable_if<std::is_integral<IntegerType>::value>::type>
class Rational {
    IntegerType numerator, denominator;

    void _Reduce() {
        if (denominator < 0) {
            denominator *= -1;
            numerator *= -1;
        }

        IntegerType temp = std::gcd(numerator, denominator);

        numerator /= temp;
        denominator /= temp;
    }

public:
    Rational() : numerator(0), denominator(1) {
    }

    Rational(IntegerType num) : numerator(num), denominator(1) {
    }

    Rational(IntegerType num, IntegerType denom) : numerator(num), denominator(denom) {
        _Reduce();
    }

    IntegerType GetNumerator() const {
        IntegerType temp = std::gcd(numerator, denominator);

        if (denominator < 0) {
            return numerator / temp * -1;
        }

        return numerator / temp;
    }

    IntegerType GetDenominator() const {
        IntegerType temp = std::gcd(numerator, denominator);
        return std::abs(denominator / temp);
    }

    std::pair<IntegerType, IntegerType> GetPair() const {
        return {numerator, denominator};
    };

    void Invert() {
        std::swap(numerator, denominator);
        _Reduce();
    }

    Rational Inverted() const {
        Rational temp = *this;

        temp.Invert();

        temp._Reduce();
        return temp;
    }

    long double Eval() const {
        return static_cast<double>(numerator) / denominator;
    }

    Rational &operator+() {
        _Reduce();
        return *this;
    }

    Rational operator-() {
        Rational temp = *this;
        temp.numerator *= -1;

        temp._Reduce();
        return temp;
    }

    Rational &operator=(const Rational &other) {
        this->numerator = other.numerator;
        this->denominator = other.denominator;

        _Reduce();
        return *this;
    }

    Rational operator+(const Rational &other) const {
        Rational result = *this;

        int64_t temp = std::lcm(result.denominator, other.denominator);
        result.numerator *= temp / result.denominator;
        result.denominator = temp;
        result.numerator += temp / other.denominator * other.numerator;

        result._Reduce();
        return result;
    }

    Rational operator-(const Rational &other) const {
        Rational result = *this;

        int64_t temp = std::lcm(result.denominator, other.denominator);
        result.numerator *= temp / result.denominator;
        result.denominator = temp;
        result.numerator -= temp / other.denominator * other.numerator;

        result._Reduce();
        return result;
    }

    Rational operator*(const Rational &other) const {
        Rational temp = *this;

        temp.numerator *= other.numerator;
        temp.denominator *= other.denominator;

        temp._Reduce();
        return temp;
    }

    Rational operator/(const Rational &other) const {
        return *this * other.Inverted();
    }

    Rational &operator+=(const Rational &other) {
        *this = *this + other;
        return *this;
    }

    Rational &operator-=(const Rational &other) {
        *this = *this - other;
        return *this;
    }

    Rational &operator*=(const Rational &other) {
        *this = *this * other;
        return *this;
    }

    Rational &operator/=(const Rational &other) {
        *this = *this / other;
        return *this;
    }

    bool operator==(const Rational &other) const {
        return numerator == other.numerator && denominator == other.denominator;
    }

    bool operator!=(const Rational &other) const {
        return numerator != other.numerator || denominator != other.denominator;
    }

    bool operator<(const Rational &other) const {
        int64_t temp = std::lcm(denominator, other.denominator);
        return (numerator * (temp / denominator)) < (other.numerator * (temp / other.denominator));
    }

    bool operator>(const Rational &other) const {
        int64_t temp = std::lcm(denominator, other.denominator);
        return (numerator * (temp / denominator)) > (other.numerator * (temp / other.denominator));
    }

    bool operator<=(const Rational &other) const {
        int64_t temp = std::lcm(denominator, other.denominator);
        return (numerator * (temp / denominator)) <= (other.numerator * (temp / other.denominator));
    }

    bool operator>=(const Rational &other) const {
        int64_t temp = std::lcm(denominator, other.denominator);
        return (numerator * (temp / denominator)) >= (other.numerator * (temp / other.denominator));
    }

    Rational operator++(int) {
        Rational temp = *this;
        *this += 1;
        return temp;
    }

    Rational &operator++() {
        *this += 1;
        return *this;
    }

    Rational operator--(int) {
        Rational temp = *this;
        *this -= 1;
        return temp;
    }

    Rational &operator--() {
        *this -= 1;
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &out, const Rational &other) {
        out << other.numerator << '/' << other.denominator;
        return out;
    }
};

template<class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
Rational<T> operator+(int64_t left, const Rational<T> &other) {
    return other + left;
}

template<class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
Rational<T> operator-(int64_t left, const Rational<T> &other) {
    return other - left;
}

template<class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
Rational<T> operator*(int64_t left, const Rational<T> &other) {
    return other * left;
}

template<class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
Rational<T> operator/(int64_t left, const Rational<T> &other) {
    return other / left;
}

} // GB namespace