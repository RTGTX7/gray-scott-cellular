#include <cadmium/modeling/celldevs/grid/coupled.hpp>
#include <cadmium/simulation/logger/csv.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <chrono>
#include <fstream>
#include <string>
#include <cmath>
#include <iostream>
#include <random>
#include "include/grayScottCell.hpp"
#include "include/grayScottState.hpp"

using namespace cadmium::celldevs;
using namespace cadmium;

// Function to add a Gray-Scott cell to the grid
std::shared_ptr<GridCell<grayScottState, double>> addGridCell(const coordinates & cellId, 
                                                              const std::shared_ptr<const GridCellConfig<grayScottState, double>>& cellConfig) {
    auto cellModel = cellConfig->cellModel;

    if (cellModel == "grayScott") {
        return std::make_shared<grayScott>(cellId, cellConfig);
    } else {
        throw std::bad_typeid();
    }
}

// Main function to initialize and run the simulation
int main(int argc, char ** argv) {
    if (argc < 2) {
        std::cout << "Program used with wrong parameters. The program must be invoked as follows:" << std::endl;
        std::cout << argv[0] << " SCENARIO_CONFIG.json [MAX_SIMULATION_TIME (default: 500)]" << std::endl;
        return -1;
    }
    
    std::string configFilePath = argv[1];
    double simTime = (argc > 2) ? std::stod(argv[2]) : 500;

    auto model = std::make_shared<GridCellDEVSCoupled<grayScottState, double>>("grayScott", addGridCell, configFilePath);
    model->buildModel();
    
    auto rootCoordinator = RootCoordinator(model);
    rootCoordinator.setLogger<CSVLogger>("grid_log.csv", ";");
    
    rootCoordinator.start();
    rootCoordinator.simulate(simTime);
    rootCoordinator.stop();
}
