#pragma once

#include <vector>
#include <stdexcept>
#include <cmath>
#include <utility>

namespace sumcheck{
namespace poly{

template<typename F>
class MultilinearPolynomial{

private:
    //存储当前层级的点值表示
    std::vector<F_ext> evals;
    size_t _num_vars;

public:
    template <typename T>
    explicit MultilinearPolynomial(const std::vector<T>& initial_evals){
        size_t n = initial_evals.size();
        //检查是否为2的幂
        if((n & (n - 1)) != 0 || n == 0){
            throw std::invalid_argument("Size must be a power of 2");
        }
        
        _num_vars = static_cast<size_t>(std::log2(n));

        evals.reserve(n);
        for(const auto& v: initial_evals){
            evals.push_back(F_ext(v));
        }
    }

    size_t num_vars() const{
        return _num_vars;
    }

    size_t current_size() const{
        return evals.size();
    }


    // 获取当前轮次的求和点
    std::pair<F, F> get_round_sums() const{
        size_t half = evals.size() / 2;
        F_ext sum_0(0);
        F_ext sum_1(0);

        for(size_t i = 0; i < half; i++){
            sum_0 += evals[i];
            sum_1 += evals[i + half];
        }

        return{sum_0, sum_1};
    }

    // 折叠
    void fix_variable(const F& r_i){
        if(_num_vars == 0) throw std::runtime_error("No variables left to fix");

        size_t half = evals.size() / 2;

        for(size_t i = 0; i < half; i++){
            evals[i] = evals[i] + (evals[i + half] - evals[i]) * r_i;
        }

        evals.resize(half);
        _num_vars--;
    }

    F Header_evaluation() const{
        if(evals.size() != 1) throw std::runtime_error("Polynomial not fully folded");
        return evals[0];
    }
};

} // namespace poly 
} // namespace goldilocks
