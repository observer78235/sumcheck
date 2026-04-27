#pragma once
#include <cstdint>
#include "../../sumcheck/transcript.hpp"

namespace babybear {
class Field {
public:
    uint32_t v;
    static constexpr uint32_t P = 0x78000001;

    Field(uint64_t val = 0) : v(val % P) {}
    Field operator+(const Field& rhs) const { uint32_t t = v + rhs.v; return Field(t >= P ? t - P : t); }
    Field operator*(const Field& rhs) const { return Field((uint64_t)v * rhs.v % P); }
    Field operator-(const Field& rhs) const { return Field(v >= rhs.v ? v - rhs.v : v + P - rhs.v); }
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

    Field inverse() const {
        return this->pow(P - 2);
    }
};
}

// 在域的头文件末尾“注册”到框架
namespace sumcheck {
template <>
struct FieldTraits<babybear::Field> {
    static void absorb(Transcript& ts, const babybear::Field& e) { ts.absorb_u64(e.v); }
    static babybear::Field squeeze(Transcript& ts) { return babybear::Field(ts.squeeze_u64() % babybear::Field::P); }
};
}
