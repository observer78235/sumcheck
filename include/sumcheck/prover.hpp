#pragma once
#include "poly/multilinear.hpp"
#include "poly/univariate.hpp"
#include "transcript.hpp"
#include "proof.hpp"

namespace goldilocks {

class SumcheckProver {
public:
    
    static SumcheckProof<GF_base, GF_2> prove(
        poly::MultilinearPolynomial<GF_base, GF_2> poly, 
        Transcript& transcript
    ) {
        size_t num_vars = poly.num_vars();
        SumcheckProof<GF_base, GF_2> proof;
        

        auto initial_sums = poly.get_round_sums();
        proof.claimed_sum = initial_sums.first + initial_sums.second;
        

        transcript.absorb_field_element(proof.claimed_sum);

        for (size_t i = 0; i < num_vars; ++i) {

            auto round_sums = poly.get_round_sums();
            poly::UnivariatePoly<GF_2> round_poly({round_sums.first, round_sums.second});
            

            proof.round_polys.push_back(round_poly);

            transcript.absorb_poly(round_poly);
            GF_2 r_i = transcript.squeeze_challenge();


            poly.fix_variable(r_i);
        }

        return proof;
    }
};

} // namespace goldilocks