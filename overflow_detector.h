#pragma once

#include <type_traits>
#include <cstdint>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <iostream>
#include <numeric>

#include "concepts.h"

namespace GB {

template<Integral IntegerType = int64_t>
class OverflowDetector {
public:

    OverflowDetector() = default;

    OverflowDetector(IntegerType value) : value_(value) {
    }

    constexpr static IntegerType GetMaxValue() noexcept {
        return kMaxValue;
    }

    constexpr static IntegerType GetMinValue() noexcept {
        return kMinValue;
    }

    explicit operator IntegerType() const noexcept {
        return value_;
    }

    OverflowDetector operator+() const noexcept {
        OverflowDetector result = *this;

        return result;
    }

    static constexpr OverflowDetector gcd(OverflowDetector lhs, OverflowDetector rhs) noexcept {
        return std::gcd(lhs.value_, rhs.value_);
    }

    static constexpr OverflowDetector lcm(OverflowDetector lhs, OverflowDetector rhs) noexcept {
        return lhs / gcd(lhs, rhs) * rhs;
    }

    static constexpr bool DoesUnaryMinusOverflow(IntegerType value) noexcept {
        constexpr IntegerType offset = kMaxValue + kMinValue;

        if (offset < 0 && value == kMinValue) {
            return true;
        }

        if (offset > 0 && value == kMaxValue) {
            return true;
        }

        return false;
    }

    OverflowDetector operator-() const noexcept {
        assert(!DoesUnaryMinusOverflow(value_));
        OverflowDetector result = *this;

        return result.value_ * -1;
    }

    static constexpr bool DoesAdditionOverflow(IntegerType lhs, IntegerType rhs) noexcept {
        if (rhs > 0 && (lhs > kMaxValue - rhs)) {
            return true;
        }
        if (rhs < 0 && (lhs < kMinValue - rhs)) {
            return true;
        }

        return false;
    }

    OverflowDetector &operator+=(const OverflowDetector &other) noexcept {
        assert(!DoesAdditionOverflow(value_, other.value_));
        value_ += other.value_;

        return *this;
    }

    static constexpr bool DoesSubtractionOverflow(IntegerType lhs, IntegerType rhs) noexcept {
        if (rhs < 0 && (lhs > kMaxValue + rhs)) {
            return true;
        }
        if (rhs > 0 && (lhs < kMinValue + rhs)) {
            return true;
        }

        return false;
    }

    OverflowDetector &operator-=(const OverflowDetector &other) noexcept {
        assert(!DoesSubtractionOverflow(value_, other.value_));
        value_ -= other.value_;

        return *this;
    }

    static constexpr bool DoesMultiplicationOverflow(IntegerType lhs, IntegerType rhs) noexcept {
        constexpr IntegerType offset = kMaxValue + kMinValue;

        if (lhs == 0 || rhs == 0) {
            return false;
        }

        if (offset > 0) {
            if (rhs == -1) {
                return lhs == kMaxValue;
            }
            if (lhs == -1) {
                return rhs == kMaxValue;
            }
        } else if (offset < 0) {
            if (rhs == -1) {
                return lhs == kMinValue;
            }
            if (lhs == -1) {
                return rhs == kMinValue;
            }
        }

        if (lhs < 0) {
            if (rhs < 0) {
                return lhs < kMaxValue / rhs;
            } else {
                return lhs < kMinValue / rhs;
            }
        } else {
            if (rhs < 0) {
                return lhs > kMinValue / rhs;
            } else {
                return lhs > kMaxValue / rhs;
            }
        }
    }

    OverflowDetector &operator*=(const OverflowDetector &other) noexcept {
        assert(!DoesMultiplicationOverflow(value_, other.value_));
        value_ *= other.value_;

        return *this;
    }

    static constexpr bool DoesDivisionOverflow(IntegerType lhs, IntegerType rhs) noexcept {
        constexpr IntegerType offset = kMaxValue + kMinValue;

        if (rhs == 0) {
            return true;
        }

        if (offset > 0) {
            if ((lhs == -1 && rhs == kMaxValue) || (lhs == kMaxValue && rhs == -1)) {
                return true;
            }
        } else if (offset < 0) {
            if ((lhs == -1 && rhs == kMinValue) || (lhs == kMinValue && rhs == -1)) {
                return true;
            }
        }

        return false;
    }

    OverflowDetector &operator/=(const OverflowDetector &other) noexcept {
        assert(!DoesDivisionOverflow(value_, other.value_));
        value_ /= other.value_;

        return *this;
    }

    friend OverflowDetector operator+(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        OverflowDetector result = lhs;
        result += rhs;

        return result;
    }

    friend OverflowDetector operator-(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        OverflowDetector result = lhs;
        result -= rhs;

        return result;
    }

    friend OverflowDetector operator*(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        OverflowDetector result = lhs;
        result *= rhs;

        return result;
    }

    friend OverflowDetector operator/(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        OverflowDetector result = lhs;
        result /= rhs;

        return result;
    }

    friend bool operator==(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        return lhs.value_ == rhs.value_;
    }

    friend bool operator!=(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        return !(lhs == rhs);
    }

    friend bool operator<(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        return lhs.value_ < rhs.value_;
    }

    friend bool operator>(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        return rhs < lhs;
    }

    friend bool operator<=(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        return !(rhs < lhs);
    }

    friend bool operator>=(const OverflowDetector &lhs, const OverflowDetector &rhs) noexcept {
        return !(lhs < rhs);
    }

    friend std::ostream &operator<<(std::ostream &out, const OverflowDetector &other) {
        out << other.value_;
        return out;
    }

private:
    static constexpr IntegerType kMinValue = std::numeric_limits<IntegerType>::min();
    static constexpr IntegerType kMaxValue = std::numeric_limits<IntegerType>::max();

    IntegerType value_;
};

} // namespace GB
