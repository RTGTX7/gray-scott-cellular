#ifndef CADMIUM_EXAMPLE_CELLDEVS_CONWAY_HPP_
#define CADMIUM_EXAMPLE_CELLDEVS_CONWAY_HPP_

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "conwayState.hpp"

using namespace cadmium::celldevs;

//! Conway game of life cell.
class conway : public GridCell<conwayState, double> {
	public:
	conway(const std::vector<int>& id, const std::shared_ptr<const GridCellConfig<conwayState, double>>& config):
		GridCell<conwayState, double>(id, config) { }

	[[nodiscard]] conwayState localComputation(conwayState state,
		const std::unordered_map<std::vector<int>, NeighborData<conwayState, double>>& neighborhood) const override {
		int live_neighbors = 0;

		for (const auto& [neighborId, neighborData]: neighborhood) {
			auto nState = neighborData.state;

			if(nState->life == true) {
				live_neighbors++;
			}
		}

		if(state.life == true) {
			live_neighbors--; //Self is a neighbor, we do not care about that yet.
			if(live_neighbors < 2 || live_neighbors > 3) {
				state.life = false;
			}
		} else {
			if(live_neighbors == 3) {
				state.life = true;
			}
		}

		return state;
	}

	[[nodiscard]] double outputDelay(const conwayState& state) const override {
		return 1.;
	}
};

#endif //CADMIUM_EXAMPLE_CELLDEVS_CONWAY_HPP_
