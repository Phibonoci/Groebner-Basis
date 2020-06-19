#pragma once

#include "concepts.h"
#include <unordered_map>

namespace GB {

template <int64_t MOD = 1'000'000'007>
class ModularInt {
public:
    ModularInt(int64_t value) : value_((value % MOD + MOD) % MOD) {
    }
    
    ModularInt() : value_(0) {
    }
    
    [[nodiscard]] int64_t GetValue() const noexcept {
        return value_;
    }

    ModularInt operator+() const noexcept {
        ModularInt result = *this;

        return result;
    }

    ModularInt operator-() const noexcept {
        ModularInt result = *this;
        result.value_ = (MOD - value_) % MOD;

        return result;
    }

    ModularInt &operator+=(const ModularInt &other) noexcept {
        value_ = (other.value_ + value_) % MOD;
        return *this;
    }

    ModularInt &operator-=(const ModularInt &other) noexcept {
        value_ = (value_ - other.value_ + MOD) % MOD;
        return *this;
    }

    ModularInt &operator*=(const ModularInt &other) noexcept {
        value_ = (other.value_ * value_) % MOD;
        return *this;
    }

    ModularInt &operator/=(const ModularInt &other) noexcept {
        value_ = (value_ * GetInverseElement(other.value_)) % MOD;
        return *this;
    }

    friend ModularInt operator+(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        ModularInt result = lhs;
        result += rhs;

        return result;
    }

    friend ModularInt operator-(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        ModularInt result = lhs;
        result -= rhs;

        return result;
    }

    friend ModularInt operator*(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        ModularInt result = lhs;
        result *= rhs;

        return result;
    }

    friend ModularInt operator/(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        ModularInt result = lhs;
        result /= rhs;

        return result;
    }

    friend bool operator==(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        return lhs.value_ == rhs.value_;
    }

    friend bool operator!=(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        return !(lhs == rhs);
    }

    friend bool operator<(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        return lhs.value_ < rhs.value_;
    }

    friend bool operator>(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        return rhs < lhs;
    }

    friend bool operator<=(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        return !(rhs < lhs);
    }

    friend bool operator>=(const ModularInt &lhs, const ModularInt &rhs) noexcept {
        return !(lhs < rhs);
    }

    friend std::ostream &operator<<(std::ostream &out, const ModularInt &other) noexcept {
        out << other.value_;
        return out;
    }

    inline static int64_t GetInverseElement(int64_t value) noexcept {
        return BinaryPower_(value, MOD - 2);
    }

private:
    int64_t value_;

    inline static int64_t BinaryPower_(int64_t value, int64_t power) noexcept {
        int64_t res = 1;
        while (power != 0) {
            if (power % 2 == 1) {
                res = (res * value) % MOD;
            }
            value = (value * value) % MOD;
            power /= 2;
        }
        return res;
    }
};

template<int64_t MOD>
ModularInt<MOD> abs(const ModularInt<MOD> &other) {
    return other.GetValue();
}

} // namespace GB
