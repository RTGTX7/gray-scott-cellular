#ifndef CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_STATE_HPP_
#define CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_STATE_HPP_

#include <iostream>
#include <nlohmann/json.hpp>

//! Gray-Scott reaction-diffusion state.
struct grayScottState {
    double u, v;
    double v_ratio;
    //! 默认构造函数：初始化 u=1.0, v=0.0, v_ratio=0.0
    grayScottState() : u(1.0), v(0.0), v_ratio(0.0) {}
};

//! 输出 u 和 v 到输出流
std::ostream& operator<<(std::ostream& os, const grayScottState& x) {
    os << "<" << x.u << ", " << x.v << ">";
    return os;
}

//! 比较两个状态对象
bool operator!=(const grayScottState& x, const grayScottState& y) {
    return x.u != y.u || x.v != y.v;
}

//! 从 JSON 解析生成 grayScottState 对象
void from_json(const nlohmann::json& j, grayScottState& s) {
    j.at("u").get_to(s.u);
    j.at("v").get_to(s.v);
    // v_ratio 后续在计算中更新，因此不必从 JSON 初始化
}

#endif // CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_STATE_HPP_
