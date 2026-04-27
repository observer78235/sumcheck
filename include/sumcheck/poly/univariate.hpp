#pragma once
#include <vector>

namespace sumcheck{
namespace poly{

template <typename Field>
class UnivariatePoly{
public:
    std::vector<Field> evals;

    UnivariatePoly() = default;
    explicit UnivariatePoly(const std::vector<Field>& points): evals(points){
         if (evals.empty()) {
            // 至少需要一个点才能定义多项式（常数）
            throw std::invalid_argument("UnivariatePoly must have at least one point");
        }
        // 验证点个数是否是 2 的幂，这里是 Sum-check 的隐式要求
        // 但在通用多项式里，如果需要，可以只要求是连续整数 {0, 1, ...}
        // 如果要严格模仿 Sum-check Prover 的输入，则此处检查是必要的
        size_t n = evals.size();
        if ((n & (n - 1)) != 0) {
             throw std::invalid_argument("UnivariatePoly for Sumcheck expects number of points to be a power of 2");
        }
    }

    //获取多项式的次数d
    size_t degree() const{
        return evals.size() - 1;
    }

    //在随机点r处求值
    Field evaluate(const Field& r) const{
        if (evals.empty()) {
            throw std::runtime_error("Cannot evaluate empty polynomial");
        }
        
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