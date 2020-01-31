#include <cassert>

#include "rational.h"
#include "overflow_detector.h"
#include "monomial.h"
#include "polynomial.h"

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
    using Term = std::pair<const Monomial, Rational<>>;
    using IndexType = size_t;
    using DegreeType = OverflowDetector<uint64_t>;

    void TestOverflow() {
        EXPECT_TRUE(IntOD::DoesUnaryMinusOverflow(INT_MIN));
        EXPECT_TRUE(IntOD::DoesAdditionOverflow(1, IntOD::GetMaxValue()));
        EXPECT_TRUE(IntOD::DoesSubtractionOverflow(-2, IntOD::GetMaxValue()));
        EXPECT_TRUE(IntOD::DoesMultiplicationOverflow(2, IntOD::GetMaxValue() / 2 + 1));
        EXPECT_TRUE(IntOD::DoesMultiplicationOverflow(2, (IntOD::GetMinValue() / 2 - 1)));
        EXPECT_TRUE(IntOD::DoesMultiplicationOverflow(-2, (IntOD::GetMinValue() / 2 - 1)));

        EXPECT_FALSE(IntOD::DoesUnaryMinusOverflow(INT_MAX));
        EXPECT_FALSE(IntOD::DoesAdditionOverflow(0, IntOD::GetMaxValue()));
        EXPECT_FALSE(IntOD::DoesSubtractionOverflow(-1, IntOD::GetMaxValue()));
        EXPECT_FALSE(IntOD::DoesMultiplicationOverflow(2, (IntOD::GetMaxValue() / 2)));
        EXPECT_FALSE(IntOD::DoesMultiplicationOverflow(2, (IntOD::GetMinValue() / 2)));
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

    void TestMonomial() {
        Monomial m0, m1({1, 2, 3}), m2({1, 0, 0, 1}), m3({1, 2, 3, 4});

        EXPECT_THROW(m1 / m2);
        EXPECT_THROW(m1 / Monomial({1, 2, 4}));

        EXPECT_NOT_THROW(m3 / m2);
        EXPECT_NOT_THROW(m0 / Monomial());

        EXPECT_EQUAL(m1, Monomial({1, 2, 3, 0}));
        EXPECT_EQUAL(m1, m3 / Monomial({0, 0, 0, 4}));
        EXPECT_EQUAL(m1, m1 * m0);
        EXPECT_EQUAL(m1 * m3, Monomial({2, 4, 6, 4}));

        EXPECT_DIFFERENT(m1, Monomial({1, 2, 2}));
        EXPECT_DIFFERENT(m1, m3 / Monomial({0, 0, 0, 3}));

        EXPECT_TRUE(m0.IsDivisibleBy(Monomial{}));
        EXPECT_TRUE(Monomial::HasNoVariables(Monomial{}));

        EXPECT_FALSE(Monomial::HasNoVariables(Monomial(m1)));
    }

    void TestPolynomial() {
        Polynomial p1({{{1, 2, 3}, 1}, {{0, 1}, 8}}), p2({1, 2, 3});

        EXPECT_EQUAL(Polynomial({{1, 2}, 0}), Polynomial{});
        EXPECT_EQUAL(p1 - p1, Polynomial{});
        EXPECT_EQUAL(p1 + p1, p1 * Polynomial(2));
        {
            Polynomial result({{{2, 4, 6}, 1}, {{1, 3, 3}, 16}, {{0, 2, 0}, 64}});
            EXPECT_EQUAL(p1 * p1, result);
        }
        EXPECT_EQUAL(Polynomial{}, -Polynomial{});
        EXPECT_EQUAL(p1, +p1);
        EXPECT_EQUAL(p1 + p2, Polynomial({{{1, 2, 3}, 2}, {{0, 1}, 8}}));
        EXPECT_EQUAL(p1 - p2, Polynomial({{{0, 1}, 8}}));
        EXPECT_EQUAL(p1.GetAmountOfTerms(), 2ull);
        EXPECT_EQUAL(p1.GetNthTerm(1), Term({1, 2, 3}, 1));

        EXPECT_DIFFERENT(p1, -p1);

        EXPECT_TRUE(Polynomial<>::IsEmpty(Polynomial{}));
    }

    void TestAll() {
        TestRational();
        TestOverflow();
        TestMonomial();
        TestPolynomial();
    }

} // namespace GB
