#ifndef CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_STATE_HPP_
#define CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_STATE_HPP_

#include <iostream>
#include <nlohmann/json.hpp>

//! Gray-Scott reaction-diffusion state.
struct grayScottState {
    double u, v;
    double v_ratio;
    //! Default constructor: initializes u=1.0, v=0.0, v_ratio=0.0
    grayScottState() : u(1.0), v(0.0), v_ratio(0.0) {}
};

//! Output u and v to the output stream
std::ostream& operator<<(std::ostream& os, const grayScottState& x) {
    os << "<" << x.u << ", " << x.v << ">";
    return os;
}

//! Compare two state objects
bool operator!=(const grayScottState& x, const grayScottState& y) {
    return x.u != y.u || x.v != y.v;
}

//! Parse a grayScottState object from JSON
void from_json(const nlohmann::json& j, grayScottState& s) {
    j.at("u").get_to(s.u);
    j.at("v").get_to(s.v);
    // v_ratio will be updated during computation, so it does not need to be initialized from JSON
}

#endif // CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_STATE_HPP_
