#ifndef CADMIUM_EXAMPLE_CELLDEVS_VEGETATION_HPP_
#define CADMIUM_EXAMPLE_CELLDEVS_VEGETATION_HPP_

#include <cmath>
#include <random>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "vegetationState.hpp"

using namespace cadmium::celldevs;

/**
 * @brief A GridCell implementation of the Rietkerk Turing vegetation model.
 * State variables: S (soil moisture), B (plant biomass).
 */
class vegetationCell : public GridCell<vegetationState, double> {
public:
    vegetationCell(
        const std::vector<int>& id,
        const std::shared_ptr<const GridCellConfig<vegetationState, double>>& config
    ) : GridCell<vegetationState, double>(id, config)
      , cell_id(id)
    {}
    //std::cerr << "Cell ["<<cell_id[0]<<","<<cell_id[1]<<"] neighbors=" << neighborhood.size() << "\n";

    [[nodiscard]] vegetationState localComputation(
        vegetationState state,
        const std::unordered_map<std::vector<int>, NeighborData<vegetationState, double>>& neighborhood
    ) const override {
        // 在 localComputation 的开头加：

//state.S = std::max(0.0, state.S);
//state.B = std::max(0.0, state.B);
        // Model parameters
        const double gamma = 1.6;
        const double sigma = 1.6;
        const double mu    = 0.2;
        const double pho   = 1.5;
        const double delta = 100.0;
        const double p     = 0.15;
        const double beta  = 3.0;
        const double dt    = 0.0005;  // Cadmium time-step
        // Physical grid spacing: length=128 over n=256 cells => dx=0.5
        const double dX2   = 0.5 * 0.5;
        double sb0 = state.S - beta * state.B;
        // Compute discrete Laplacian for S - beta*B and for B
        double lap_Sb = 0.0;
        double lap_B  = 0.0;
        for (const auto& kv : neighborhood) {
            const auto& nb = *(kv.second.state);
            lap_Sb += (nb.S - beta * nb.B);
            lap_B  += nb.B;
        }
        // subtract central cell
        double N = static_cast<double>(neighborhood.size());
        lap_Sb = (lap_Sb - N * sb0) / dX2;
        lap_B  = (lap_B  - N * state.B) / dX2;
        

        // Reaction-diffusion equations
        double dSdt = p
            - (1.0 - pho * state.B) * state.S
            - state.S * state.S * state.B
            + delta * lap_Sb;
        double dBdt = (gamma * state.S / (1.0 + sigma * state.S)) * state.B
            - state.B * state.B
            - mu * state.B
            + lap_B;
            if (!std::isfinite(dSdt) || !std::isfinite(dBdt)) {
                std::cerr<<"  >>> bad deriv at cell "<<cell_id[0]<<","<<cell_id[1]
                         <<": dSdt="<<dSdt<<", dBdt="<<dBdt<<"\n";
            }
        // Euler integration
        state.S += dSdt * dt;
        state.B += dBdt * dt;
        if (!std::isfinite(state.S) || !std::isfinite(state.B)) {
            std::cerr<<"  >>> overflow after update at cell "<<cell_id[0]<<","<<cell_id[1]
                     <<": S="<<state.S<<", B="<<state.B<<"\n";
        }
        return state;
    }

    [[nodiscard]] double outputDelay(const vegetationState&) const override {
        return 1;
    }

private:
    std::vector<int> cell_id;
};

#endif // CADMIUM_EXAMPLE_CELLDEVS_VEGETATION_HPP_