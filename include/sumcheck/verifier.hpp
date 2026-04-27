#pragma once

#include <vector>
#include "proof.hpp"          
#include "transcript.hpp"     
#include "poly/multilinear.hpp"

namespace sumcheck{

template <typename F>
class Verifier {
public:
    static bool verify(
        const SumcheckProof<F>& proof,
        size_t num_vars,
        const poly::MultilinearPolynomial<F>& original_poly,
        Transcript& transcript
    ) {

        F current_target_sum = proof.claimed_sum;
        transcript.absorb_field_element(proof.claimed_sum);
        
        std::vector<F> challenges; 
        challenges.reserve(num_vars);


        for (size_t i = 0; i < num_vars; ++i) {
            const auto& g_i = proof.round_polys[i];

            if (g_i.evals[0] + g_i.evals[1] != current_target_sum) {
                return false; // 发现作弊！
            }


            transcript.absorb_poly(g_i);
            F r_i = transcript.squeeze_challenge();
            challenges.push_back(r_i);


            current_target_sum = g_i.evaluate(r_i);
        }


        F actual_eval = evaluate_mle_at_point(original_poly, challenges);

        return actual_eval == current_target_sum;
    }

private:

    static F evaluate_mle_at_point(
        poly::MultilinearPolynomial<F> poly, 
        const std::vector<F>& r
    ) {

        for (const auto& r_i : r) {
            poly.fix_variable(r_i);
        }
        return poly.final_evaluation();
    }
};

} // namespace goldilocks