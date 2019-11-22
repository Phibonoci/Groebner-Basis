#pragma once

#include <type_traits>
#include <cstdint>

#include <cmath>
#include <limits>
#include <stdexcept>

namespace GB {

template<class IntegerType = int64_t, class = typename std::enable_if_t<std::is_integral_v<IntegerType>>>
class OverflowDetector {
public:

    OverflowDetector(IntegerType value) : value_(value) {
    }

    constexpr static IntegerType GetMaxValue() {
        return std::numeric_limits<IntegerType>::max();
    }

    constexpr static IntegerType GetMinValue() {
        return std::numeric_limits<IntegerType>::min();
    }

    explicit operator IntegerType() const {
        return value_;
    }

    OverflowDetector operator+() const {
        OverflowDetector result = *this;

        return result;
    }

    static constexpr bool IsUnaryMinusOverflow(IntegerType value) {
        constexpr IntegerType offset = maxValue_ + minValue_;

        if (offset < 0 && value == minValue_) {
            return true;
        }

        if (offset > 0 && value == maxValue_) {
            return true;
        }

        return false;
    }

    OverflowDetector operator-() const {
        assert(!IsUnaryMinusOverflow(value_));
        OverflowDetector result = *this;

        return result.value_ * -1;
    }

    static constexpr bool IsAdditionOverflow(IntegerType lhs, IntegerType rhs) {
        if (rhs > 0 && (lhs > maxValue_ - rhs)) {
            return true;
        }
        if (rhs < 0 && (lhs < minValue_ - rhs)) {
            return true;
        }

        return false;
    }

    OverflowDetector &operator +=(const OverflowDetector& other) {
        assert(!IsAdditionOverflow(value_, other.value_));
        value_ += other.value_;

        return *this;
    }

    static constexpr bool IsSubtractionOverflow(IntegerType lhs, IntegerType rhs) {
        if (rhs < 0 && (lhs > maxValue_ + rhs)) {
            return true;
        }
        if (rhs > 0 && (lhs < minValue_ + rhs)) {
            return true;
        }

        return false;
    }

    OverflowDetector &operator -=(const OverflowDetector& other) {
        assert(!IsSubtractionOverflow(value_, other.value_));
        value_ -= other.value_;

        return *this;
    }

    static constexpr bool IsMultiplicationOverflow(IntegerType lhs, IntegerType rhs) {
        constexpr IntegerType offset = maxValue_ + minValue_;

        if (lhs == 0 || rhs == 0) {
            return false;
        }

        if (offset > 0) {
            if ((rhs == -1 && lhs == maxValue_) || (rhs == maxValue_ && lhs == -1)) {
                return true;
            }
        } else if (offset < 0) {
            if ((rhs == -1 && lhs == minValue_) || (rhs == minValue_ && lhs == -1)) {
                return true;
            }
        }

        if (lhs < 0) {
            if (rhs < 0) {
                return lhs < maxValue_ / rhs;
            } else {
                return lhs < minValue_ / rhs;
            }
        } else {
            if (rhs < 0) {
                return lhs > minValue_ / rhs;
            } else {
                return lhs > maxValue_ / rhs;
            }
        }
    }

    OverflowDetector &operator *=(const OverflowDetector& other) {
        assert(!IsMultiplicationOverflow(value_, other.value_));
        value_ *= other.value_;

        return *this;
    }

    static constexpr bool IsDivisionOverflow(IntegerType lhs, IntegerType rhs) {
        constexpr IntegerType offset = maxValue_ + minValue_;

        if (rhs == 0) {
            return true;
        }

        if (offset > 0) {
            if ((lhs == -1 && rhs == maxValue_) || (lhs == maxValue_ && rhs == -1)) {
                return true;
            }
        } else if (offset < 0) {
            if ((lhs == -1 && rhs == minValue_) || (lhs == minValue_ && rhs == -1)) {
                return true;
            }
        }

        return false;
    }

    OverflowDetector &operator /=(const OverflowDetector& other) {
        assert(!IsDivisionOverflow(value_, other.value_));
        value_ /= other.value_;

        return *this;
    }

    friend OverflowDetector operator+(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        OverflowDetector result = lhs;
        result += rhs;

        return result;
    }

    friend OverflowDetector operator-(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        OverflowDetector result = lhs;
        result -= rhs;

        return result;
    }

    friend OverflowDetector operator*(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        OverflowDetector result = lhs;
        result *= rhs;

        return result;
    }

    friend OverflowDetector operator/(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        OverflowDetector result = lhs;
        result /= rhs;

        return result;
    }

    friend bool operator==(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        return lhs.value_ == rhs.value_;
    }

    friend bool operator!=(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        return lhs.value_ < rhs.value_;
    }

    friend bool operator>(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        return !(rhs < lhs);
    }

    friend bool operator>=(const OverflowDetector &lhs, const OverflowDetector &rhs) {
        return !(lhs < rhs);
    }
    
private:
    static constexpr IntegerType minValue_ = std::numeric_limits<IntegerType>::min();
    static constexpr IntegerType maxValue_ = std::numeric_limits<IntegerType>::max();

    IntegerType value_;
};

} // namespace GB
