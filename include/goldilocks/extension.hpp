#pragma once

#include "field.hpp"

namespace goldilocks{

class GF_2{
    public:
    GF_base re;
    GF_base im; 


    GF_2(GF_base r = 0, GF_base i = 0) : re(r), im(i) {}
    
    GF_2(uint64_t val) : re(val), im(0) {}


    GF_2 operator+(const GF_2& rhs) const {
        return GF_2(re + rhs.re, im + rhs.im);
    }

    GF_2 operator-(const GF_2& rhs) const {
        return GF_2(re - rhs.re, im - rhs.im);
    }


    GF_2 operator*(const GF_2& rhs) const {
        GF_base ac = re * rhs.re;
        GF_base bd = im * rhs.im;
        GF_base ad = re * rhs.im;
        GF_base bc = im * rhs.re;
        
        return GF_2(ac + bd * GF_base(7), ad + bc);
    }

    GF_2& operator+=(const GF_2& rhs) { *this = *this + rhs; return *this; }
    GF_2& operator-=(const GF_2& rhs) { *this = *this - rhs; return *this; }
    GF_2& operator*=(const GF_2& rhs) { *this = *this * rhs; return *this; }


    GF_2 inverse() const {
        if (re.v == 0 && im.v == 0) {
            return GF_2(0, 0);
        }
        
        GF_base denom = (re * re) - (im * im * GF_base(7));
        
        GF_base inv_denom = denom.inverse(); 
        
        return GF_2(re * inv_denom, GF_base(0) - (im * inv_denom));
    }

    bool operator==(const GF_2& rhs) const {
        return re == rhs.re && im == rhs.im;
    }

    bool operator!=(const GF_2& rhs) const {
        return !(*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const GF_2& rhs) {
        return os << "(" << rhs.re << " + " << rhs.im << "i)";
    }
};

}