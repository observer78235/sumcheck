#include <iostream>
#include <vector>

// 包含所有域
#include "fields/goldilocks/field.hpp"
#include "fields/babybear/field.hpp"
#include "fields/koalabear/field.hpp"

// 包含协议逻辑
#include "sumcheck/prover.hpp"
#include "sumcheck/verifier.hpp"

template <typename F>
void run_sumcheck_test(const std::string& field_name, const std::vector<uint64_t>& raw_data) {
    std::cout << "\n--- Testing Sum-check on " << field_name << " ---" << std::endl;
    
    // 1. 构造多项式
    std::vector<F> evals;
    for (auto d : raw_data) evals.push_back(F(d));
    sumcheck::poly::MultilinearPolynomial<F> poly(evals);

    // 2. 证明者生成证明
    sumcheck::Transcript prover_ts("Sumcheck_Demo");
    auto proof = sumcheck::Prover<F>::prove(poly, prover_ts);
    std::cout << "Claimed Sum: " << proof.claimed_sum << std::endl;

    // 3. 验证者验证
    sumcheck::Transcript verifier_ts("Sumcheck_Demo");
    bool ok = sumcheck::Verifier<F>::verify(proof, poly.num_vars(), poly, verifier_ts);

    if (ok) std::cout << "Result: ACCEPTED" << std::endl;
    else std::cout << "Result: REJECTED" << std::endl;
}

int main() {
    std::vector<uint64_t> data = {1, 2, 3, 4, 5, 6, 7, 8};

    // 运行 Goldilocks (64-bit)
    run_sumcheck_test<goldilocks::GF_base>("Goldilocks", data);

    // 运行 BabyBear (31-bit)
    run_sumcheck_test<babybear::Field>("BabyBear", data);

    // 运行 KoalaBear (31-bit)
    run_sumcheck_test<koalabear::Field>("KoalaBear", data);

    return 0;
}
