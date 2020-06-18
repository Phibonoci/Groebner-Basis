#pragma once

#include "monomial.h"

namespace GB {

struct LexicographicalOrder {
    bool operator()(const Monomial &lhs, const Monomial &rhs) const {
        return lhs.GetDegrees() < rhs.GetDegrees();
    }
};

struct ReverseLexicographicalOrder {
    bool operator()(const Monomial &lhs, const Monomial &rhs) const {
        return rhs.GetDegrees() < lhs.GetDegrees();
    }
};

struct GradedLexicographicalOrder {
    bool operator()(const Monomial &lhs, const Monomial &rhs) const {
        auto lTotalDegree = lhs.TotalDegree();
        auto rTotalDegree = rhs.TotalDegree();

        if (lTotalDegree == rTotalDegree) {
            LexicographicalOrder order;
            return order(lhs, rhs);
        }

        return lTotalDegree < rTotalDegree;
    }
};

struct GradedReverseLexicographicalOrder {
    bool operator()(const Monomial &lhs, const Monomial &rhs) const {
        auto lTotalDegree = lhs.TotalDegree();
        auto rTotalDegree = rhs.TotalDegree();

        if (lTotalDegree == rTotalDegree) {
            ReverseLexicographicalOrder revOrder;
            return revOrder(lhs, rhs);
        }

        return lTotalDegree < rTotalDegree;
    }
};

} // namespace GB
