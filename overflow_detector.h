#pragma once

#include <type_traits>
#include <cstdint>

#include <cmath>
#include <limits>
#include <stdexcept>

namespace GB {

using namespace std::string_literals;

template<class IntegerType = int64_t, class = typename std::enable_if_t<std::is_integral_v<IntegerType>>>
class OverflowDetector {
public:

    OverflowDetector(IntegerType value) : value_(value) {
    }

    IntegerType GetValue() const {
        return value_;
    }

    static IntegerType GetMaxValue() {
        return std::numeric_limits<IntegerType>::max();
    }

    static IntegerType GetMinValue() {
        return std::numeric_limits<IntegerType>::min();
    }

    OverflowDetector operator+() const {
        OverflowDetector result = *this;
        return result;
    }

    OverflowDetector operator-() const {
        OverflowDetector result = *this;

        IntegerType offset = maxValue_ + minValue_;

        if (offset < 0 && value_ == minValue_) {
            throw std::overflow_error("");
        }

        if (offset > 0 && value_ == maxValue_) {
            throw std::underflow_error("");
        }

        return result.value_ * -1;
    }

    OverflowDetector &operator +=(const OverflowDetector& other) {
        if (other > 0 && (value_ > maxValue_ - other.value_)) {
            throw std::overflow_error("");
        }
        if (other < 0 && (value_ < minValue_ + other.value_)) {
            throw std::underflow_error("");
        }

        value_ += other.value_;
        return *this;
    }

    OverflowDetector &operator -=(const OverflowDetector& other) {
        if (other < 0 && (value_ > maxValue_ - other.value_)) {
            throw std::overflow_error("");
        }
        if (other > 0 && (value_ < minValue_ + other.value_)) {
            throw std::underflow_error("");
        }

        value_ -= other.value_;
        return *this;
    }

    OverflowDetector &operator *=(const OverflowDetector& other) {
        IntegerType offset = maxValue_ + minValue_;

        if (offset > 0) {
            if ((other.value_ == -1 && value_ == maxValue_) || (other.value_ == maxValue_ && value_ == -1)) {
                throw std::underflow_error("");
            }
        } else if (offset < 0) {
            if ((other.value_ == -1 && value_ == minValue_) || (other.value_ == minValue_ && value_ == -1)) {
                throw std::overflow_error("");
            }
        }

        if ((value_ < 0 && other.value_ < 0) || (value_ > 0 && other.value_ > 0)) {
            if (value_ > maxValue_ / other.value_) {
                throw std::overflow_error("");
            }
        }

        if ((value_ > 0 && other.value_ < 0) || (value_ < 0 && other.value_ > 0)) {
            if (value_ > minValue_ / other.value_) {
                throw std::underflow_error("");
            }
        }

        value_ *= other.value_;

        return *this;
    }

    OverflowDetector &operator /=(const OverflowDetector& other) {
        IntegerType offset = maxValue_ + minValue_;

        if (other.value_ == 0) {
            throw std::overflow_error("");
        }

        if (offset > 0) {
            if ((value_ == -1 && other.value_ == maxValue_) || (value_ == maxValue_ && other.value_ == -1)) {
                throw std::underflow_error("");
            }
        } else if (offset < 0) {
            if ((value_ == -1 && other.value_ == minValue_) || (value_ == minValue_ && other.value_ == -1)) {
                throw std::overflow_error("");
            }
        }



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
    IntegerType value_;

    IntegerType minValue_ = std::numeric_limits<IntegerType>::min();
    IntegerType maxValue_ = std::numeric_limits<IntegerType>::max();
};

} // namespace GB
