#pragma once
#include "poly/multilinear.hpp"
#include "poly/univariate.hpp"
#include "transcript.hpp"
#include "proof.hpp"

namespace sumcheck {

template <typename F>
class Prover {
public:
    
    static SumcheckProof<F> prove(
        poly::MultilinearPolynomial<F> poly, 
        Transcript& transcript
    ) {
        size_t num_vars = poly.num_vars();
        SumcheckProof<F> proof;
        

        auto initial_sums = poly.get_round_sums();
        proof.claimed_sum = initial_sums.first + initial_sums.second;
        

        transcript.absorb_field_element(proof.claimed_sum);

        for (size_t i = 0; i < num_vars; ++i) {

            auto round_sums = poly.get_round_sums();
            poly::UnivariatePoly<F> round_poly({round_sums.first, round_sums.second});
            

            proof.round_polys.push_back(round_poly);

            transcript.absorb_poly(round_poly);
            F r_i = transcript.squeeze_challenge_as<F>();


            poly.fix_variable(r_i);
        }

        return proof;
    }
};

} // namespace goldilocks