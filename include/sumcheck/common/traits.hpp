#pragma once
#include <cstdint>

namespace sumcheck {

// 向前声明 Transcript，以便在 Trait 中使用
class Transcript;

/**
 * @brief 域特征模板。每个具体的域都需要特化这个模板。
 */
template <typename F>
struct FieldTraits {
    // 静态接口：如何吸纳一个元素
    // static void absorb(Transcript& ts, const F& element);
    
    // 静态接口：如何挤出一个挑战值
    // static F squeeze(Transcript& ts);
};

} // namespace sumcheck
