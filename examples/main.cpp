#include <iostream>
#include <vector>
#include <string>

// 包含所有基础域
#include "fields/goldilocks/field.hpp"
#include "fields/babybear/field.hpp"
#include "fields/koalabear/field.hpp"

// 包含所有扩域定义
#include "fields/goldilocks/extension.hpp"
#include "fields/babybear/extension.hpp"
#include "fields/koalabear/extension.hpp"

// 包含协议逻辑
#include "sumcheck/prover.hpp"
#include "sumcheck/verifier.hpp"

// 保持你原来的测试模板函数
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
    // 测试数据：2^3 = 8 个点
    std::vector<uint64_t> data = {1, 2, 3, 4, 5, 6, 7, 8};

    // --- 1. Goldilocks 验证 ---
    run_sumcheck_test<goldilocks::GF_base>("Goldilocks Base", data);
    run_sumcheck_test<goldilocks::GF_2>("Goldilocks Ext2", data);
    run_sumcheck_test<goldilocks::GF_4>("Goldilocks Ext4", data);

    // --- 2. BabyBear 验证 ---
    run_sumcheck_test<babybear::Field>("BabyBear Base", data);
    run_sumcheck_test<babybear::Ext2>("BabyBear Ext2", data);
    run_sumcheck_test<babybear::Ext4>("BabyBear Ext4", data);

    // --- 3. KoalaBear 验证 ---
    run_sumcheck_test<koalabear::Field>("KoalaBear Base", data);
    run_sumcheck_test<koalabear::Ext2>("KoalaBear Ext2", data);
    run_sumcheck_test<koalabear::Ext4>("KoalaBear Ext4", data);

    return 0;
}
