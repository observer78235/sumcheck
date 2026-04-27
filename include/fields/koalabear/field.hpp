#pragma once
#include <cstdint>
#include <iostream>
#include "../../sumcheck/transcript.hpp"

namespace koalabear {

/**
 * @brief KoalaBear Field GF(p)
 * p = 2^31 - 2^24 + 1 = 2130706433
 */
class Field {
public:
    uint32_t v;
    static constexpr uint32_t P = 0x7F000001;

    Field(uint64_t val = 0) : v(static_cast<uint32_t>(val % P)) {}

    Field operator+(const Field& rhs) const {
        uint32_t res = v + rhs.v;
        return Field(res >= P ? res - P : res);
    }

    Field operator-(const Field& rhs) const {
        return Field(v >= rhs.v ? v - rhs.v : v + P - rhs.v);
    }

    Field operator*(const Field& rhs) const {
        // 31位 * 31位 = 62位，uint64_t 足以容纳
        return Field((static_cast<uint64_t>(v) * rhs.v) % P);
    }

    // 快速幂
    Field pow(uint64_t exp) const {
        Field res(1);
        Field base = *this;
        while (exp > 0) {
            if (exp % 2 == 1) res = res * base;
            base = base * base;
            exp /= 2;
        }
        return res;
    }

    // 逆元 (费马小定理)
    Field inverse() const {
        return pow(P - 2);
    }

    bool operator==(const Field& rhs) const { return v == rhs.v; }
    bool operator!=(const Field& rhs) const { return v != rhs.v; }

    friend std::ostream& operator<<(std::ostream& os, const Field& rhs) {
        return os << rhs.v;
    }
};

} // namespace koalabear

// --- 注册到 Sum-check 框架的 Traits ---
namespace sumcheck {
template <>
struct FieldTraits<koalabear::Field> {
    static void absorb(Transcript& ts, const koalabear::Field& e) {
        // 31位元素可以直接作为 64位吸纳
        ts.absorb_u64(static_cast<uint64_t>(e.v));
    }
    
    static koalabear::Field squeeze(Transcript& ts) {
        // 挤出一个 64位随机数并取模
        return koalabear::Field(ts.squeeze_u64() % koalabear::Field::P);
    }
};
}
