#include <cassert>

#include "rational.h"
#include "monomial.h"
#include "polynomial.h"
#include "algorithms.h"

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
    using Term = Polynomial<>::Term;
    using IndexType = Monomial::IndexType;
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
        EXPECT_DIFFERENT(p1, -p1);

        EXPECT_TRUE(Polynomial<>::IsZero(Polynomial {}));

        {
            Polynomial f1(Term{{1, 2}, 16});
            Polynomial f2(Term{{1, 2}, -10});

            EXPECT_EQUAL(f1 - f2, Polynomial(Term{{1, 2}, 26}));
            EXPECT_EQUAL(f1 + f2, Polynomial(Term{{1, 2}, 6}));
        }
    }

    void TestOrder() {
        Polynomial<Rational<>, LexicographicalOrder> lexOrder({
            {{1, 2, 3}, 1},
            {{1, 2, 4}, 1},
            {{2, 2, 2}, 1},
            {{6, 0, 0}, 1}
        });

        Polynomial<Rational<>, ReverseLexicographicalOrder> revLexOrder = lexOrder;
        Polynomial<Rational<>, GradedLexicographicalOrder> gradedLexOrder = lexOrder;
        Polynomial<Rational<>, GradedReverseLexicographicalOrder> gradedRevLexOrder = lexOrder;

        EXPECT_EQUAL(lexOrder.GetNthTerm(0), Term({6, 0, 0}, 1));
        EXPECT_EQUAL(lexOrder.GetNthTerm(1), Term({2, 2, 2}, 1));
        EXPECT_EQUAL(lexOrder.GetNthTerm(2), Term({1, 2, 4}, 1));
        EXPECT_EQUAL(lexOrder.GetNthTerm(3), Term({1, 2, 3}, 1));

        EXPECT_EQUAL(revLexOrder.GetNthTerm(0), Term({1, 2, 3}, 1));
        EXPECT_EQUAL(revLexOrder.GetNthTerm(1), Term({1, 2, 4}, 1));
        EXPECT_EQUAL(revLexOrder.GetNthTerm(2), Term({2, 2, 2}, 1));
        EXPECT_EQUAL(revLexOrder.GetNthTerm(3), Term({6, 0, 0}, 1));

        EXPECT_EQUAL(gradedLexOrder.GetNthTerm(0), Term({1, 2, 4}, 1));
        EXPECT_EQUAL(gradedLexOrder.GetNthTerm(1), Term({6, 0, 0}, 1));
        EXPECT_EQUAL(gradedLexOrder.GetNthTerm(2), Term({2, 2, 2}, 1));
        EXPECT_EQUAL(gradedLexOrder.GetNthTerm(3), Term({1, 2, 3}, 1));

        EXPECT_EQUAL(gradedRevLexOrder.GetNthTerm(0), Term({1, 2, 4}, 1));
        EXPECT_EQUAL(gradedRevLexOrder.GetNthTerm(1), Term({1, 2, 3}, 1));
        EXPECT_EQUAL(gradedRevLexOrder.GetNthTerm(2), Term({2, 2, 2}, 1));
        EXPECT_EQUAL(gradedRevLexOrder.GetNthTerm(3), Term({6, 0, 0}, 1));
    }

    void TestAlgorithms() {
        {
            Polynomial p1({{{1, 1}, 1}, {{1}, 2}, {{0, 0, 1}, -1}});
            Polynomial p2({{{2}, 1}, {{0, 1}, 2}, {{0, 0, 1}, -1}});
            Polynomial expectedSPoly({{{2}, 2}, {{1, 0, 1}, -1}, {{0, 2}, -2}, {{0, 1, 1}, 1}});
            EXPECT_EQUAL(SPolynomial(p1, p2), expectedSPoly);
        }

        {
            Polynomial f({{1, 1, 1}, 1});
            Polynomial g({{{1, 1}, 1}, {{0, 0, 0, 1}, -1}});
            Polynomial expectedPolyReduction({{0, 0, 1, 1}, 1});
            EXPECT_TRUE(ElementaryReduction(f, g));
            EXPECT_EQUAL(f, expectedPolyReduction);
        }

        {
            Polynomial f({{1, 1, 1}, 1});
            Polynomial g({{{1, 0, 1}, 1}, {{0, 0, 0, 1}, -1}});
            Polynomial expectedPolyReduction({{0, 1, 0, 1}, 1});
            EXPECT_TRUE(ElementaryReduction(f, g));
            EXPECT_EQUAL(f, expectedPolyReduction);
        }

        {
            Polynomial f({{1, 0}, 1});
            Polynomial g({{0, 1}, 1});
            EXPECT_FALSE(ElementaryReduction(f, g));
        }

        {
            Polynomial f1 = Polynomial(Term{{2}, 1}) + Polynomial(Term{{1}, -2}) +
                           Polynomial(Term{{0, 2}, 1}) + Polynomial(Term{{0, 1}, -26}) + Polynomial(70);
            Polynomial f2 = Polynomial(Term{{2}, 1}) + Polynomial(Term{{1}, -22}) +
                           Polynomial(Term{{0, 2}, 1}) + Polynomial(Term{{0, 1}, -16}) + Polynomial(160);
            Polynomial f3 = Polynomial(Term{{2}, 1}) + Polynomial(Term{{1}, -20}) +
                           Polynomial(Term{{0, 2}, 1}) + Polynomial(Term{{0, 1}, -2}) + Polynomial(76);

            PolynomialSet<> set = {f1, f2, f3};

            BuhbergerAlgorithm(set);

            for (const auto &i : set) {
                std::cout << i << '\n';
            }
        }

        {
            Polynomial f1 = Polynomial(Term{{2}, -5}) - Polynomial(Term{{1, 1}, 4}) - Polynomial(Term{{1}, 2}) + Polynomial(Term{{0, 2}, 2});
            Polynomial f2 = -Polynomial(Term{{1, 1}, 4}) - Polynomial(Term{{1}, 2}) + Polynomial(Term{{0, 2}, 2}) + Polynomial(5);

            PolynomialSet<> set = {f1, f2};

            BuhbergerAlgorithm(set);

            for (const auto &i : set) {
                std::cout << i << '\n';
            }
        }
    }

    void TestAll() {
        TestRational();
        TestOverflow();
        TestMonomial();
        TestPolynomial();
        TestOrder();
        TestAlgorithms();
    }

} // namespace GB
