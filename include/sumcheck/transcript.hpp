#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "field.hpp"
#include "extension.hpp"
#include "poly/univariate.hpp"

namespace goldilocks {

class Transcript {
private:

    uint64_t state[4];

    void permute() {

        for (int i = 0; i < 8; ++i) {
            state[0] += state[1]; state[3] ^= state[0]; state[3] = (state[3] << 16) | (state[3] >> 48);
            state[2] += state[3]; state[1] ^= state[2]; state[1] = (state[1] << 12) | (state[1] >> 52);
            state[0] += state[1]; state[3] ^= state[0]; state[3] = (state[3] << 8)  | (state[3] >> 56);
            state[2] += state[3]; state[1] ^= state[2]; state[1] = (state[1] << 7)  | (state[1] >> 57);
            

            uint64_t tmp = state[0]; state[0] = state[2]; state[2] = tmp;
        }
    }

public:
 
    explicit Transcript(const std::string& label) {
        state[0] = 0x0123456789ABCDEFULL;
        state[1] = 0xFEDCBA9876543210ULL;
        state[2] = 0x8888888888888888ULL;
        state[3] = 0x7777777777777777ULL;


        for (char c : label) {
            absorb_u64(static_cast<uint64_t>(c));
        }
        permute();
    }

    void absorb_u64(uint64_t x) {
        state[0] ^= x;  
        permute();      
    }

    void absorb_field_element(const GF_base& e) {
        absorb_u64(e.v);
    }

    void absorb_field_element(const GF_2& e) {
        absorb_u64(e.re.v);
        absorb_u64(e.im.v);
    }

    template <typename Field>
    void absorb_poly(const poly::UnivariatePoly<Field>& poly) {
        for (const auto& eval : poly.evals) {
            absorb_field_element(eval);
        }
    }


    uint64_t squeeze_u64() {
        permute();     
        return state[0]; 
    }


    GF_2 squeeze_challenge() {
        uint64_t r0_raw = squeeze_u64();
        uint64_t r1_raw = squeeze_u64();

        GF_base r0(r0_raw % GF_base::P);
        GF_base r1(r1_raw % GF_base::P);

        return GF_2(r0, r1);
    }
};

} // namespace goldilocks