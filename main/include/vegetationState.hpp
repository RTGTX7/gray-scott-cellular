// include/vegetationState.hpp
#ifndef CADMIUM_EXAMPLE_CELLDEVS_VEGETATION_STATE_HPP_
#define CADMIUM_EXAMPLE_CELLDEVS_VEGETATION_STATE_HPP_

#include <iostream>
#include <nlohmann/json.hpp>

//! Turing vegetation state: S = soil moisture, B = biomass
struct vegetationState {
    double S;
    double B;
    vegetationState(): S(1.0), B( (std::rand()%10<1) ? 2.0 : 0.0 ) {} 
};

//! Stream‐output for logging
inline std::ostream& operator<<(std::ostream& os, const vegetationState& x) {
    os << "<"<<x.B << ", " << x.S <<">";
    return os;
}

//! Parse from JSON (if you want to seed differently)
inline void from_json(const nlohmann::json& j, vegetationState& s) {
    j.at("S").get_to(s.S);
    j.at("B").get_to(s.B);
}
inline bool operator!=(const vegetationState& a, const vegetationState& b) {
    return a.S != b.S || a.B != b.B;
}
#endif // CADMIUM_EXAMPLE_CELLDEVS_VEGETATION_STATE_HPP_
