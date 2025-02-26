#ifndef CADMIUM_EXAMPLE_CELLDEVS_CONWAY_STATE_HPP_
#define CADMIUM_EXAMPLE_CELLDEVS_CONWAY_STATE_HPP_

#include <iostream>
#include <nlohmann/json.hpp>

//! Conway game of life cell state.
struct conwayState {
	bool life;

	//! Default constructor function.
	conwayState() : life(false) {}
};

//! The simulator must be able to compare the equality of two state objects
bool operator!=(const conwayState& x, const conwayState& y) {
	return x.life != y.life;
}

//! It prints the life of the cell in an output stream.
std::ostream& operator<<(std::ostream& os, const conwayState& x) {
	os << "<" << ((x.life)? 1 : 0) << ">";
	return os;
}

//! It parses a JSON file and generates the corresponding conwayState object.
void from_json(const nlohmann::json& j, conwayState& s) {
	j.at("life").get_to(s.life);
}

#endif //CADMIUM_EXAMPLE_CELLDEVS_CONWAY_STATE_HPP_
