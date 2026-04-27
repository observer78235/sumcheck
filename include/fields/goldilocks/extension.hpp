#pragma once

#include "field.hpp"
#include "../../sumcheck/transcript.hpp"


namespace goldilocks {

/**
 * @brief 二次扩域 GF(p^2)
 * i^2 = 7
 */
class GF_2 {
public:
    GF_base re, im;
    GF_2(GF_base r = 0, GF_base i_val = 0) : re(r), im(i_val) {}
    GF_2(uint64_t val) : re(val), im(0) {}

    // 特殊优化：乘以虚数单位 i
    // (a + bi) * i = ai + bi^2 = 7b + ai
    GF_2 mul_by_i() const {
        return GF_2(im * GF_base(7), re);
    }

    GF_2 operator+(const GF_2& rhs) const { return {re + rhs.re, im + rhs.im}; }
    GF_2 operator-(const GF_2& rhs) const { return {re - rhs.re, im - rhs.im}; }
    
    GF_2 operator*(const GF_2& rhs) const {
        GF_base ac = re * rhs.re;
        GF_base bd = im * rhs.im;
        GF_base ad_bc = (re + im) * (rhs.re + rhs.im) - ac - bd;
        return {ac + bd * GF_base(7), ad_bc};
    }

    GF_2 inverse() const {
        GF_base denom = (re * re) - (im * im * GF_base(7));
        GF_base inv_denom = denom.inverse();
        return {re * inv_denom, GF_base(0) - (im * inv_denom)};
    }

    bool operator==(const GF_2& rhs) const { return re == rhs.re && im == rhs.im; }
    bool operator!=(const GF_2& rhs) const { return !(*this == rhs); }
};

/**
 * @brief 四次扩域 GF(p^4) - 塔式扩张
 * j^2 = i
 */
class GF_4 {
public:
    GF_2 low, high; // 元素表示为 low + high * j

    GF_4(GF_2 l = 0, GF_2 h = 0) : low(l), high(h) {}
    GF_4(uint64_t val) : low(val), high(0) {}

    GF_4 operator+(const GF_4& rhs) const { return {low + rhs.low, high + rhs.high}; }
    GF_4 operator-(const GF_4& rhs) const { return {low - rhs.low, high - rhs.high}; }

    /**
     * (A + Bj) * (C + Dj) = AC + ADj + BCj + BDj^2
     *                    = (AC + BD * i) + (AD + BC)j
     */
    GF_4 operator*(const GF_4& rhs) const {
        GF_2 AC = low * rhs.low;
        GF_2 BD = high * rhs.high;
        
        // 利用 Karatsuba 优化 A*D + B*C
        GF_2 AD_BC = (low + high) * (rhs.low + rhs.high) - AC - BD;
        
        // BD * j^2 即 BD * i
        return {AC + BD.mul_by_i(), AD_BC};
    }

    /**
     * (A + Bj)^-1 = (A - Bj) / (A^2 - B^2 * j^2)
     *            = (A - Bj) / (A^2 - B^2 * i)
     */
    GF_4 inverse() const {
        GF_2 denom = (low * low) - (high * high).mul_by_i();
        GF_2 inv_denom = denom.inverse();
        return {low * inv_denom, GF_2(0) - (high * inv_denom)};
    }

    bool operator==(const GF_4& rhs) const { return low == rhs.low && high == rhs.high; }
    bool operator!=(const GF_4& rhs) const { return !(*this == rhs); }
};

} // namespace goldilocks

// --- 注册到 Sum-check 框架的 Traits ---
namespace sumcheck {

// 注册 GF_2
template <>
struct FieldTraits<goldilocks::GF_2> {
    static void absorb(Transcript& ts, const goldilocks::GF_2& e) {
        ts.absorb_field_element(e.re);
        ts.absorb_field_element(e.im);
    }
    static goldilocks::GF_2 squeeze(Transcript& ts) {
        return goldilocks::GF_2(
            ts.squeeze_challenge_as<goldilocks::GF_base>(),
            ts.squeeze_challenge_as<goldilocks::GF_base>()
        );
    }
};

// 注册 GF_4
template <>
struct FieldTraits<goldilocks::GF_4> {
    static void absorb(Transcript& ts, const goldilocks::GF_4& e) {
        // 递归吸纳 low 和 high (它们是 GF_2)
        ts.absorb_field_element(e.low);
        ts.absorb_field_element(e.high);
    }
    static goldilocks::GF_4 squeeze(Transcript& ts) {
        // 递归挤出两个 GF_2 挑战值
        return goldilocks::GF_4(
            ts.squeeze_challenge_as<goldilocks::GF_2>(),
            ts.squeeze_challenge_as<goldilocks::GF_2>()
        );
    }
};

} // namespace sumcheck
