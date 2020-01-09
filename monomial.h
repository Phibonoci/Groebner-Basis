#pragma once

#include "rational.h"

#include <vector>

namespace GB {

template<class FieldType = Rational<DefaultIntegerType>>
class Monomial {
public:

// TODO

private:
    std::vector<DefaultOverflowDetectedType> degrees_;
    FieldType coefficient_;
};

} // namespace GB