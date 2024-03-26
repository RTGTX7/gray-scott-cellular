#ifndef CADMIUM_EXAMPLE_CELLDEVS_SIR_STATE_HPP_
#define CADMIUM_EXAMPLE_CELLDEVS_SIR_STATE_HPP_

#include <iostream>
#include <nlohmann/json.hpp>

namespace cadmium::celldevs::example::sir {
	//! Susceptible-Infected-Recovered state.
	struct SIRState {
		int state;

		//! Default constructor function. By default, cells are unoccupied and all the population is considered susceptible.
		SIRState() : state(false) {
		}
	};

	//! It returns true if x and y are different.
	inline bool operator!=(const SIRState& x, const SIRState& y) {
		return x.state != y.state;
	}

	//! It prints a SIR state in an output stream.
	std::ostream& operator<<(std::ostream& os, const SIRState& x) {
		os << "<" << x.state << ">";
		return os;
	}

	//! It parses a JSON file and generates the corresponding SIR state object.
	[[maybe_unused]] void from_json(const nlohmann::json& j, SIRState& s) {
		j.at("state").get_to(s.state);
	}
}  // namespace cadmium::celldevs::example::sir

#endif //CADMIUM_EXAMPLE_CELLDEVS_SIR_STATE_HPP_
