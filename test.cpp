#include <cassert>

#include "rational.h"
#include "overflow_detector.h"

#define EXPECT_TRUE(expression) assert(!!(expression))

#define EXPECT_FALSE(expression) assert(!(expression))

#define EXPECT_THROW(expression) \
    do { \
        try { \
            expression; \
            assert(false); \
        } catch (...) { } \
    } while (false)

#define EXPECT_NOT_THROW(expression) \
    do { \
        try { \
            expression; \
        } catch (...) { \
            assert(false); \
        } \
    } while (false)

#define EXPECT_EQUAL(lhs, rhs) assert((lhs) == (rhs))

#define EXPECT_DIFFERENT(lhs, rhs) assert((lhs) != (rhs))

namespace GB {

    using IntOD = OverflowDetector<int>;
    using LongOD = OverflowDetector<long long>;

    void TestOverflow() {
        EXPECT_TRUE(IntOD::IsUnaryMinusOverflow(INT_MIN));
        EXPECT_TRUE(IntOD::IsAdditionOverflow(1, IntOD::GetMaxValue()));
        EXPECT_TRUE(IntOD::IsSubtractionOverflow(-2, IntOD::GetMaxValue()));
        EXPECT_TRUE(IntOD::IsMultiplicationOverflow(2, IntOD::GetMaxValue() / 2 + 1));
        EXPECT_TRUE(IntOD::IsMultiplicationOverflow(2, (IntOD::GetMinValue() / 2 - 1)));
        EXPECT_TRUE(IntOD::IsMultiplicationOverflow(-2, (IntOD::GetMinValue() / 2 - 1)));

        EXPECT_FALSE(IntOD::IsUnaryMinusOverflow(INT_MAX));
        EXPECT_FALSE(IntOD::IsAdditionOverflow(0, IntOD::GetMaxValue()));
        EXPECT_FALSE(IntOD::IsSubtractionOverflow(-1, IntOD::GetMaxValue()));
        EXPECT_FALSE(IntOD::IsMultiplicationOverflow(2, (IntOD::GetMaxValue() / 2)));
        EXPECT_FALSE(IntOD::IsMultiplicationOverflow(2, (IntOD::GetMinValue() / 2)));
    }

    void TestRational() {
        EXPECT_THROW(Rational(0, 1).GetInverted());
        EXPECT_THROW(Rational(1, 0));

        EXPECT_EQUAL(Rational(2, 4), Rational(1, 2));
        EXPECT_EQUAL(Rational(0, 5), Rational(0));

        EXPECT_EQUAL(Rational(2, 3) / Rational(3), Rational(2, 3)  / 3);
        EXPECT_EQUAL(Rational(2, 3) / Rational(3), Rational(2, 3)  / 3);
        EXPECT_EQUAL(-1 / Rational(-3), Rational(1, 3));
        EXPECT_EQUAL(Rational(1, 3) - Rational(2, 6), 0);
        EXPECT_EQUAL(Rational(-1, 2) * Rational(-2, 3), Rational(1, 3));
        EXPECT_EQUAL(Rational(-1, 2) + Rational(1, 3), Rational(-1, 6));

        EXPECT_EQUAL(-Rational(-1, 3), Rational(1, 3));
        EXPECT_EQUAL(Rational(-1, 2), Rational(1, -2));
        EXPECT_EQUAL(Rational(-1, -1), 1);
        EXPECT_EQUAL(+Rational(1, 3), Rational(1, 3));

        EXPECT_EQUAL(Rational(1, 2).GetInverted(), 2);

        EXPECT_DIFFERENT(Rational(1), 0);
        EXPECT_DIFFERENT(Rational(-1), Rational(1));

        EXPECT_TRUE(Rational(1, 3) >= Rational(1, 4));
        EXPECT_TRUE(Rational(1, 3) >= Rational(1, 3));
        EXPECT_TRUE(Rational(1, 3) <= Rational(1, 3));
        EXPECT_TRUE(Rational(1, 3) <= Rational(1, 2));

        EXPECT_FALSE(Rational(1) < Rational(1));
        EXPECT_FALSE(Rational(1) > Rational(1));
    }

    void TestAll() {
        TestRational();
        TestOverflow();
    }

} // namespace GB
