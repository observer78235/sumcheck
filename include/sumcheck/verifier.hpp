#pragma once

#include <vector>
#include "field.hpp"          
#include "extension.hpp"  
#include "proof.hpp"          
#include "transcript.hpp"     
#include "poly/multilinear.hpp"

namespace goldilocks {

class SumcheckVerifier {
public:
    
    static bool verify(
        const SumcheckProof<GF_base, GF_2>& proof,
        size_t num_vars,
        const poly::MultilinearPolynomial<GF_base, GF_2>& original_poly,
        Transcript& transcript
    ) {

        GF_2 current_target_sum = proof.claimed_sum;
        transcript.absorb_field_element(proof.claimed_sum);
        
        std::vector<GF_2> challenges; 
        challenges.reserve(num_vars);


        for (size_t i = 0; i < num_vars; ++i) {
            const auto& g_i = proof.round_polys[i];

            if (g_i.evals[0] + g_i.evals[1] != current_target_sum) {
                return false; // 发现作弊！
            }


            transcript.absorb_poly(g_i);
            GF_2 r_i = transcript.squeeze_challenge();
            challenges.push_back(r_i);


            current_target_sum = g_i.evaluate(r_i);
        }


        GF_2 actual_eval = evaluate_mle_at_point(original_poly, challenges);

        return actual_eval == current_target_sum;
    }

private:

    static GF_2 evaluate_mle_at_point(
        poly::MultilinearPolynomial<GF_base, GF_2> poly, 
        const std::vector<GF_2>& r
    ) {

        for (const auto& r_i : r) {
            poly.fix_variable(r_i);
        }
        return poly.final_evaluation();
    }
};

} // namespace goldilocks
