#include <cadmium/modeling/celldevs/grid/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/logger.hpp>
#include <chrono>
#include <fstream>
#include <string>
#include <cmath>
#include <iostream>
#include <random>
#include <iomanip>
#include "include/grayScottCell.hpp"
#include "include/grayScottState.hpp"

using namespace cadmium::celldevs;
using namespace cadmium;

// Custom CSV Logger: logs output only when time is an integer multiple of logInterval.
class CustomCSVLogger : public cadmium::Logger {
public:
    std::ofstream out;
    std::string delimiter;
    double logInterval; // Logging interval in time units

    CustomCSVLogger(const std::string& filename, const std::string& delim, double interval)
        : delimiter(delim), logInterval(interval) {
        out.open(filename);
        if (!out.is_open()){
            std::cerr << "Error opening file " << filename << std::endl;
        }
        // Write CSV header
        out << "time" << delimiter << "model_id" << delimiter 
            << "model_name" << delimiter << "port_name" << delimiter 
            << "data" << "\n";
    }

    virtual void start() override {
        // Optional initialization code
    }

    virtual void stop() override {
        if (out.is_open()){
            out.close();
        }
    }

    // Log output only if time is a multiple of logInterval
    virtual void logOutput(double t, long modelId, const std::string& modelName,
                           const std::string& portName, const std::string& data) override {
        if (std::fabs(std::fmod(t, logInterval)) < 1e-9) {
            out << t << delimiter << modelId << delimiter << modelName 
                << delimiter << portName << delimiter << data << "\n";
        }
    }

    // Log state only if time is a multiple of logInterval
    virtual void logState(double t, long modelId, const std::string& modelName,
                          const std::string& state) override {
        if (std::fabs(std::fmod(t, logInterval)) < 1e-9) {
            out << t << delimiter << modelId << delimiter << modelName 
                << delimiter << "" << delimiter << state << "\n";
        }
    }

    ~CustomCSVLogger() {
        if (out.is_open()){
            out.close();
        }
    }
};

// Function to add a Gray-Scott cell to the grid.
std::shared_ptr<GridCell<grayScottState, double>> addGridCell(
    const coordinates & cellId, 
    const std::shared_ptr<const GridCellConfig<grayScottState, double>>& cellConfig) {

    if (cellConfig->cellModel == "grayScott") {
        return std::make_shared<grayScott>(cellId, cellConfig);
    } else {
        throw std::bad_typeid();
    }
}

int main(int argc, char ** argv) {
    // Check command-line arguments
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] 
                  << " SCENARIO_CONFIG.json [MAX_SIMULATION_TIME (default: 500)]" << std::endl;
        return -1;
    }
    
    std::string configFilePath = argv[1];
    double simTime = (argc > 2) ? std::stod(argv[2]) : 500;

    // Build the model from configuration
    auto model = std::make_shared<GridCellDEVSCoupled<grayScottState, double>>(
        "grayScott", addGridCell, configFilePath);
    model->buildModel();

    // Create the simulation coordinator
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger<CustomCSVLogger>("grid_log.csv", ";", 10);
    
    rootCoordinator.start();

    // Run simulation in increments and display a simple progress bar.
    double stepInterval = 10;  // progress update interval
    double currentTime = 0;
    while (currentTime < simTime) {
        currentTime += stepInterval;
        if (currentTime > simTime) currentTime = simTime;
        // Advance simulation to currentTime
        rootCoordinator.simulate(currentTime);
        // Calculate progress percentage
        double progress = (currentTime / simTime) * 100.0;
        // Print progress bar
        int barWidth = 50;
        int pos = static_cast<int>(barWidth * progress / 100.0);
        std::cout << "\rProgress: [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1)
                  << progress << "%, t = " << currentTime << "/" << simTime;
        std::cout.flush();
    }
    std::cout << "\nSimulation completed at t = " << currentTime << std::endl;

    rootCoordinator.stop();
    return 0;
}
