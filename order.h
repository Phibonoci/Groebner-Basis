#pragma once

#include "monomial.h"

namespace GB {
    struct LexicographicalOrder {
        bool operator()(const Monomial &lhs, const Monomial &rhs) const {
            return lhs < rhs;
        }
    };

    struct ReverseLexicographicalOrder {
        bool operator()(const Monomial &lhs, const Monomial &rhs) const {
            return rhs < lhs;
        }
    };

    struct GradedLexicographicalOrder {
        bool operator()(const Monomial &lhs, const Monomial &rhs) const {
            auto lTotalDegree = lhs.TotalDegree();
            auto rTotalDegree = rhs.TotalDegree();

            if (lTotalDegree == rTotalDegree) {
                return lhs < rhs;
            }

            return lTotalDegree < rTotalDegree;
        }
    };

    struct GradedReverseLexicographicalOrder {
        bool operator()(const Monomial &lhs, const Monomial &rhs) const {
            auto lTotalDegree = lhs.TotalDegree();
            auto rTotalDegree = rhs.TotalDegree();

            if (lTotalDegree == rTotalDegree) {
                return rhs < lhs;
            }

            return lTotalDegree < rTotalDegree;
        }
    };
}
