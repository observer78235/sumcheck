#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "common/traits.hpp"

namespace sumcheck {

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
        state[0] = 0x0123456789ABCDEFULL; state[1] = 0xFEDCBA9876543210ULL;
        state[2] = 0x8888888888888888ULL; state[3] = 0x7777777777777777ULL;
        for (char c : label) absorb_u64(static_cast<uint64_t>(c));
        permute();
    }

    void absorb_u64(uint64_t x) {
        state[0] ^= x;
        permute();
    }

    uint64_t squeeze_u64() {
        permute();
        return state[0];
    }

    // --- 通用接口：通过 Trait 派发 ---
    template <typename F>
    void absorb_field_element(const F& e) {
        FieldTraits<F>::absorb(*this, e);
    }

    template <typename F>
    F squeeze_challenge_as() {
        return FieldTraits<F>::squeeze(*this);
    }

    // 兼容之前的吸纳多项式接口
    template <typename F>
    void absorb_poly(const poly::UnivariatePoly<F>& poly) {
        for (const auto& eval : poly.evals) absorb_field_element(eval);
    }
};

} // namespace sumcheck