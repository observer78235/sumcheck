#pragma once
#include "field.hpp"
#include "../../sumcheck/transcript.hpp"

namespace babybear {

class Ext2 {
public:
    Field re, im;
    static constexpr uint32_t NON_RESIDUE = 11;

    Ext2(Field r = 0, Field i_val = 0) : re(r), im(i_val) {}
    Ext2(uint64_t r = 0) : re(r), im(0) {}
    Ext2 mul_by_i() const { return {im * Field(NON_RESIDUE), re}; }

    Ext2 operator+(const Ext2& rhs) const { return {re + rhs.re, im + rhs.im}; }
    Ext2 operator-(const Ext2& rhs) const { return {re - rhs.re, im - rhs.im}; }
    Ext2 operator*(const Ext2& rhs) const {
        Field ac = re * rhs.re;
        Field bd = im * rhs.im;
        Field ad_bc = (re + im) * (rhs.re + rhs.im) - ac - bd;
        return {ac + bd * Field(NON_RESIDUE), ad_bc};
    }
    Ext2 inverse() const {
        Field denom = (re * re) - (im * im * Field(NON_RESIDUE));
        Field inv_denom = denom.inverse();
        return {re * inv_denom, Field(0) - (im * inv_denom)};
    }
    bool operator==(const Ext2& rhs) const { return re == rhs.re && im == rhs.im; }
};

class Ext4 {
public:
    Ext2 low, high;
    Ext4(Ext2 l = 0, Ext2 h = 0) : low(l), high(h) {}
    Ext4(uint64_t val) : low(val), high(0) {}

    Ext4 operator+(const Ext4& rhs) const { return {low + rhs.low, high + rhs.high}; }
    Ext4 operator-(const Ext4& rhs) const { return {low - rhs.low, high - rhs.high}; }
    Ext4 operator*(const Ext4& rhs) const {
        Ext2 AC = low * rhs.low;
        Ext2 BD = high * rhs.high;
        Ext2 AD_BC = (low + high) * (rhs.low + rhs.high) - AC - BD;
        return {AC + BD.mul_by_i(), AD_BC};
    }
    Ext4 inverse() const {
        Ext2 denom = (low * low) - (high * high).mul_by_i();
        Ext2 inv_denom = denom.inverse();
        return {low * inv_denom, Ext2(0) - (high * inv_denom)};
    }
    bool operator==(const Ext4& rhs) const { return low == rhs.low && high == rhs.high; }
};
}

namespace sumcheck {
template <> struct FieldTraits<babybear::Ext2> {
    static void absorb(Transcript& ts, const babybear::Ext2& e) { ts.absorb_field_element(e.re); ts.absorb_field_element(e.im); }
    static babybear::Ext2 squeeze(Transcript& ts) { return {ts.squeeze_challenge_as<babybear::Field>(), ts.squeeze_challenge_as<babybear::Field>()}; }
};
template <> struct FieldTraits<babybear::Ext4> {
    static void absorb(Transcript& ts, const babybear::Ext4& e) { ts.absorb_field_element(e.low); ts.absorb_field_element(e.high); }
    static babybear::Ext4 squeeze(Transcript& ts) { return {ts.squeeze_challenge_as<babybear::Ext2>(), ts.squeeze_challenge_as<babybear::Ext2>()}; }
};
}