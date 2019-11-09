#include <type_traits>
#include <cstdint>
#include <cmath>
#include <numeric>
#include <iostream>

template <class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
class Rational {
    T numerator, denominator;

public:
    Rational(): numerator(0), denominator(1) {
    }

    Rational(T num): numerator(num), denominator(1) {
    }

    Rational(T num, T denom): numerator(num), denominator(denom) {
        reduce();
    }

    void reduce() {
        if (denominator < 0) {
            denominator *= -1;
            numerator *= -1;
        }

        T temp = std::gcd(numerator, denominator);

        numerator /= temp;
        denominator /= temp;
    }

    T get_numerator() const {
        T temp = std::gcd(numerator, denominator);

        if (denominator < 0) {
            return numerator / temp * -1;
        }

        return numerator / temp;
    }

    T get_denominator() const {
        T temp = std::gcd(numerator, denominator);
        return std::abs(denominator / temp);
    }

    std::pair<T, T> get_pair() const {
        return {numerator, denominator};
    };

    Rational& operator + () {
        reduce();
        return *this;
    }

    Rational operator - () {
        Rational temp = *this;
        temp.numerator *= -1;

        temp.reduce();
        return temp;
    }

    Rational& operator = (const Rational& other) {
        this->numerator = other.numerator;
        this->denominator = other.denominator;

        reduce();
        return *this;
    }

    Rational operator + (const Rational& other) const {
        Rational result = *this;

        int64_t temp = std::lcm(result.denominator, other.denominator);
        result.numerator *= temp / result.denominator;
        result.denominator = temp;
        result.numerator += temp / other.denominator * other.numerator;

        result.reduce();
        return result;
    }

    Rational operator - (const Rational& other) const {
        Rational result = *this;

        int64_t temp = std::lcm(result.denominator, other.denominator);
        result.numerator *= temp / result.denominator;
        result.denominator = temp;
        result.numerator -= temp / other.denominator * other.numerator;

        result.reduce();
        return result;
    }

    void reverse() {
        std::swap(numerator, denominator);
        reduce();
    }

    Rational reversed() const {
        Rational temp = *this;

        temp.reverse();

        temp.reduce();
        return temp;
    }

    Rational operator * (const Rational& other) const {
        Rational temp = *this;

        temp.numerator *= other.numerator;
        temp.denominator *= other.denominator;

        temp.reduce();
        return temp;
    }

    Rational operator / (const Rational& other) const {
        return *this * other.reversed();
    }

    Rational& operator += (const Rational& other) {
        *this = *this + other;
        return *this;
    }

    Rational& operator -= (const Rational& other) {
        *this = *this - other;
        return *this;
    }

    Rational& operator *= (const Rational& other) {
        *this = *this * other;
        return *this;
    }

    Rational& operator /= (const Rational& other) {
        *this = *this / other;
        return *this;
    }

    bool operator == (const Rational& other) const {
        return numerator == other.numerator && denominator == other.denominator;
    }

    bool operator != (const Rational& other) const {
        return numerator != other.numerator || denominator != other.denominator;
    }

    bool operator < (const Rational& other) const {
        int64_t temp = std::lcm(denominator, other.denominator);
        return (numerator * (temp / denominator)) < (other.numerator * (temp / other.denominator));
    }

    bool operator > (const Rational& other) const {
        int64_t temp = std::lcm(denominator, other.denominator);
        return (numerator * (temp / denominator)) > (other.numerator * (temp / other.denominator));
    }

    bool operator <= (const Rational& other) const {
        int64_t temp = std::lcm(denominator, other.denominator);
        return (numerator * (temp / denominator)) <= (other.numerator * (temp / other.denominator));
    }

    bool operator >= (const Rational& other) const {
        int64_t temp = std::lcm(denominator, other.denominator);
        return (numerator * (temp / denominator)) >= (other.numerator * (temp / other.denominator));
    }

    Rational operator ++(int) {
        Rational temp = *this;
        *this += 1;
        return temp;
    }

    Rational& operator ++() {
        *this += 1;
        return *this;
    }

    Rational operator --(int) {
        Rational temp = *this;
        *this -= 1;
        return temp;
    }

    Rational& operator --() {
        *this -= 1;
        return *this;
    }

    friend std::ostream& operator << (std::ostream& out, const Rational& other) {
        out << other.numerator << '/' << other.denominator;
        return out;
    }

    long double eval() const {
        return static_cast<double>(numerator) / denominator;
    }
};

template <class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
Rational<T> operator + (int64_t left, const Rational<T>& other) {
    return other + left;
}

template <class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
Rational<T> operator - (int64_t left, const Rational<T>& other) {
    return other - left;
}

template <class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
Rational<T> operator * (int64_t left, const Rational<T>& other) {
    return other * left;
}

template <class T = int64_t, class = typename std::enable_if<std::is_integral<T>::value>::type>
Rational<T> operator / (int64_t left, const Rational<T>& other) {
    return other / left;
}
