#pragma once
#include <vector>

namespace goldilocks{
namespace poly{

template <typename Field>
class UnivariatePoly{
public:
    std::vector<Field> evals;

    UnivariatePoly() = default;
    UnivariatePoly(const std::vector<Field>& points): evals(points){}

    //获取多项式的次数d
    size_t degree() const{
        return evals.size() - 1;
    }

    //在随机点r处求值
    Field evaluate(const Field& r) const{
        if(evals.size() == 2){
            return evals[0] * (Field(1) - r) + evals[1] * r;
        }

        Field result(0);
        size_t n = evals.size();

        for(size_t i = 0; i < n; i++){
            Field weight(1);
            Field num(1);
            Field den(1);

            for(size_t j = 0; j < n; j++){
                if(i == j) continue;

                num *= (r - Field(j));

                den *= (Field(i) - Field(j));
            }

            result += evals[i] * num * den.inverse();
        }

        return result;
    }

};

}
}