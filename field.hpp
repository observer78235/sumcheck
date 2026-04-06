#pragma once


#include <iostream>
#include <cstdint>

namespace goldilocks{


class GF_base{
    
public:
    uint64_t v;

    static constexpr uint64_t P = 0xFFFFFFFF00000001ULL;

    GF_base(){
        v = 0;
    }

    GF_base(uint64_t val){
        v = val > P ? val % P : val;
    }

    static inline uint64_t fast_reduce(unsigned __int128 x){
        uint64_t x0 = (uint32_t)x;
        uint64_t x1 = (uint32_t)(x >> 32);
        uint64_t x2 = (uint32_t)(x >> 64);
        uint64_t x3 = (uint32_t)(x >> 96);

        uint64_t lower_part = x0;
        uint64_t subtract_part = x3 + x2;
        uint64_t res1;

        if(lower_part >= subtract_part){
            res1 = lower_part - subtract_part;
        }else{
            res1 = P - subtract_part + lower_part;
        }

        uint64_t sum_12 = x1 + x2;
        uint64_t h_part = sum_12 >> 32;
        uint64_t l_part = sum_12 << 32;

        uint64_t res2 = l_part + (h_part << 32) - h_part;

        uint64_t res = res1 + res2;

        if(res < res1){
            res += (0ULL - P);
        }

        if(res >= P){
            res -= P;
        }

        return res;
    }

    GF_base operator+(const GF_base& rhs)const{
        uint64_t sum = rhs.v + v;
        if(sum < v || sum >= P){
            sum += (0ULL -P);
        }
        return GF_base(sum);
    }

    GF_base operator+=(const GF_base& rhs){
        v += rhs.v;
        if(v < rhs.v || v >= P){
            v += (0ULL -P);
        }
        return *this;
    }

    GF_base operator-(const GF_base& rhs)const{
        return GF_base(rhs.v > v ?  (P - rhs.v + v) : v - rhs.v);
    }

    GF_base operator-=(const GF_base& rhs){
        v = (rhs.v > v ? (P - rhs.v + v) : v - rhs.v);
        return *this;
    }

    GF_base operator*(const GF_base& rhs)const{
        unsigned __int128 prod = static_cast<unsigned __int128>(v) * rhs.v;
        return GF_base(fast_reduce(prod));
    }

    GF_base operator*=(const GF_base& rhs){
        unsigned __int128 prod = static_cast<unsigned __int128>(v) * rhs.v;
        v = fast_reduce(prod);
        return *this;
    }

    GF_base pow(uint64_t exp) const {
        GF_base res(1);
        GF_base base = *this;
        while (exp > 0) {
            if (exp % 2 == 1) res = res * base;
            base = base * base;
            exp /= 2;
        }
        return res;
    }

    GF_base inverse() const {
        return this->pow(P - 2);
    }

    bool operator==(const GF_base& rhs)const{
        return v == rhs.v;
    }

    bool operator!=(const GF_base& rhs)const{
        return v != rhs.v;
    }

    friend std::ostream& operator<<(std::ostream& os, const GF_base& rhs){
        return os << rhs.v;
    }
};

}