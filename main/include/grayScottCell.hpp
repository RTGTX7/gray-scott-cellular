#ifndef CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_HPP_
#define CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_HPP_

#include <cmath>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "grayScottState.hpp"

using namespace cadmium::celldevs;

/**
 * @brief A GridCell implementation of the Gray-Scott reaction-diffusion model.
 * This model simulates chemical reactions and diffusion across a 2D grid using Cell-DEVS.
 */
class grayScott : public GridCell<grayScottState, double> {
public:
    /**
     * @brief Constructor for the Gray-Scott cell.
     * @param id The coordinates of this cell in the grid.
     * @param config Configuration containing cell parameters and model metadata.
     */
    grayScott(const std::vector<int>& id,
              const std::shared_ptr<const GridCellConfig<grayScottState, double>>& config)
        : GridCell<grayScottState, double>(id, config),
          cell_id(id)
    {
    }

    /**
     * @brief Local computation logic for each simulation step.
     * Applies diffusion and reaction equations to update the chemical concentrations.
     * @param state The current state of the cell.
     * @param neighborhood A map of neighboring cell states and their positions.
     * @return The updated state of the cell.
     */
    [[nodiscard]] grayScottState localComputation(
        grayScottState state,
        const std::unordered_map<std::vector<int>, NeighborData<grayScottState, double>>& neighborhood
    ) const override {
        
        //-------------------------
        // 1. Reaction-diffusion parameters
        //-------------------------
        double f = 0.055;   // Feed rate
        double k = 0.117;   // Kill rate
        double r = 1.0;    // Reaction rate constant

        double dA = 1;     // Diffusion rate of chemical A (u)
        double dB = 0.5;   // Diffusion rate of chemical B (v)

        constexpr double dt = 1;  // Simulation time step

        //-------------------------
        // 2. Calculate Laplacian (diffusion) using weighted 3x3 kernel
        // Kernel:
        // [[0.05, 0.2, 0.05],
        //  [0.2 , -1 , 0.2 ],
        //  [0.05, 0.2, 0.05]]
        //-------------------------
        double laplacian_u = 0.0;
        double laplacian_v = 0.0;

        for (const auto& [neighborId, neighborData] : neighborhood) {
            int dx = neighborId[0] - cell_id[0];
            int dy = neighborId[1] - cell_id[1];
            double weight = 0.0;

            if (std::abs(dx) == 1 && std::abs(dy) == 1) {
                weight = 0.05;  // Diagonal neighbors
            } else if ((std::abs(dx) == 1 && dy == 0) || (std::abs(dy) == 1 && dx == 0)) {
                weight = 0.2;   // Direct neighbors (up/down/left/right)
            }

            laplacian_u += weight * neighborData.state->u;
            laplacian_v += weight * neighborData.state->v;
        }

        // Subtract central cell contribution
        laplacian_u += -1.0 * state.u;
        laplacian_v += -1.0 * state.v;

        //-------------------------
        // 3. Compute reaction and diffusion terms
        //-------------------------
        double du_diff = dA * laplacian_u;
        double dv_diff = dB * laplacian_v;

        double feed_term = f * (1.0 - state.u);
        double kill_term = k * state.v;
        double reaction_term = r * state.u * state.v * state.v;

        double du_dt = du_diff + feed_term - reaction_term;
        double dv_dt = dv_diff - kill_term + reaction_term;

        //-------------------------
        // 4. Update state using Euler integration
        //-------------------------
        state.u += du_dt * dt;
        state.v += dv_dt * dt;

        //-------------------------
        // 5. Calculate v_ratio = v / (u + v), avoid division by zero
        //-------------------------
        double sum_uv = state.u + state.v;
        
        if (sum_uv > 0.0) {
            state.v_ratio = state.v / sum_uv;
        } else {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0.0, 1.0);
            state.v_ratio = dis(gen);
        }

        return state;
    }

    /**
     * @brief Specifies the output delay between local computations.
     * @return The output delay (fixed at 1.0 time unit).
     */
    [[nodiscard]] double outputDelay(const grayScottState& state) const override {
        return 1.0;
    }

protected:
    /// The coordinates of this cell, used to determine neighbor positions.
    std::vector<int> cell_id;
};

#endif // CADMIUM_EXAMPLE_CELLDEVS_GRAYSCOTT_HPP_
