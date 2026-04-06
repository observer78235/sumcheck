#include <iostream>
#include <vector>
#include <cassert>

// 包含你的库文件
#include "goldilocks/field.hpp"
#include "goldilocks/extension.hpp"
#include "goldilocks/poly/multilinear.hpp"
#include "goldilocks/prover.hpp"
#include "goldilocks/verifier.hpp"
#include "goldilocks/transcript.hpp"

using namespace goldilocks;
using namespace goldilocks::poly;

int main() {
    std::cout << "=== Goldilocks Sum-check Protocol Demo ===" << std::endl;

    // --- 1. 准备阶段 ---
    // 我们定义一个 3 变量的多项式 g(x1, x2, x3)
    // 在布尔超立方体 {0,1}^3 上的取值为 {1, 2, 3, 4, 5, 6, 7, 8}
    // 数学上的总和应该是：1+2+3+4+5+6+7+8 = 36
    std::vector<GF_base> evals = {
        GF_base(1), GF_base(2), GF_base(3), GF_base(4),
        GF_base(5), GF_base(6), GF_base(7), GF_base(8)
    };
    size_t num_vars = 3;

    // 构造 MLE 多项式对象
    MultilinearPolynomial<GF_base, GF_2> poly(evals);

    // --- 2. 证明者 (Prover) 生成证明 ---
    std::cout << "\n[Prover] Generating proof for sum = 36..." << std::endl;
    
    // 初始化 Transcript，给定一个协议标签
    Transcript prover_ts("Sumcheck_Example_v1");
    
    // 生成证明（注意：prove 会消耗 poly 的副本，因为内部要进行折叠）
    SumcheckProof<GF_base, GF_2> proof = SumcheckProver::prove(poly, prover_ts);

    std::cout << "[Prover] Proof generated successfully." << std::endl;
    std::cout << "[Prover] Claimed Sum: " << proof.claimed_sum << std::endl;

    // --- 3. 验证者 (Verifier) 验证证明 ---
    std::cout << "\n[Verifier] Starting verification..." << std::endl;
    
    // 验证者必须使用相同的初始标签初始化 Transcript
    Transcript verifier_ts("Sumcheck_Example_v1");
    
    // 执行验证
    bool is_valid = SumcheckVerifier::verify(proof, num_vars, poly, verifier_ts);

    if (is_valid) {
        std::cout << "[Verifier] ACCEPT: The proof is mathematically valid!" << std::endl;
    } else {
        std::cout << "[Verifier] REJECT: Invalid proof!" << std::endl;
        return -1;
    }

    // --- 4. 安全性测试：尝试篡改证明 ---
    std::cout << "\n[Security Test] Attacking: Tampering with the claimed sum..." << std::endl;
    
    SumcheckProof<GF_base, GF_2> bad_proof = proof;
    // 恶意修改总和：把 36 改成 37
    bad_proof.claimed_sum = GF_2(GF_base(37), GF_base(0));

    Transcript attack_ts("Sumcheck_Example_v1");
    bool attack_result = SumcheckVerifier::verify(bad_proof, num_vars, poly, attack_ts);

    if (!attack_result) {
        std::cout << "[Security Test] SUCCESS: Verifier detected the fraud!" << std::endl;
    } else {
        std::cout << "[Security Test] FAILURE: Verifier was fooled! (Check your code logic)" << std::endl;
    }

    return 0;
}
