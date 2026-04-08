#pragma once
#include <vector>
#include "poly/univariate.hpp"

namespace goldilocks{


template <typename F_base, typename F_ext>
struct SumcheckProof{
    F_ext claimed_sum;

    std::vector<poly::UnivariatePoly<F_ext>> round_polys;
};


}