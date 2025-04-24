#include <cadmium/modeling/celldevs/grid/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/logger.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

#include "include/vegetationCell.hpp"
#include "include/vegetationState.hpp"

using namespace cadmium::celldevs;
using namespace cadmium;

// -----------------------
// Custom CSV Logger
// -----------------------
class CustomCSVLogger : public Logger {
public:
    std::ofstream out;
    std::string delimiter;
    double logInterval;

    CustomCSVLogger(const std::string& filename,
                    const std::string& delim,
                    double interval)
        : delimiter(delim), logInterval(interval) {
        out.open(filename);
        if (!out.is_open()) {
            std::cerr << "Error opening log file: " << filename << std::endl;
        }
        // CSV header
        out << "time" << delimiter
            << "model_id" << delimiter
            << "model_name" << delimiter
            << "port_name" << delimiter
            << "data" << "\n";
    }

    void start() override {}
    void stop() override {
        if (out.is_open()) out.close();
    }

    void logOutput(double t,
                   long modelId,
                   const std::string& modelName,
                   const std::string& portName,
                   const std::string& data) override {
        if (std::fabs(std::fmod(t, logInterval)) < 1e-9) {
            out << t << delimiter
                << modelId << delimiter
                << modelName << delimiter
                << portName << delimiter
                << data << "\n";
        }
    }

    void logState(double t,
                  long modelId,
                  const std::string& modelName,
                  const std::string& state) override {
        if (std::fabs(std::fmod(t, logInterval)) < 1e-9) {
            out << t << delimiter
                << modelId << delimiter
                << modelName << delimiter
                << ""         << delimiter
                << state      << "\n";
        }
    }

    ~CustomCSVLogger() {
        if (out.is_open()) out.close();
    }
};

// Factory: create vegetation cells
static std::shared_ptr<GridCell<vegetationState, double>> addGridCell(
    const std::vector<int>& cellId,
    const std::shared_ptr<const GridCellConfig<vegetationState, double>>& cellConfig) {
    if (cellConfig->cellModel == "vegetation") {
        return std::make_shared<vegetationCell>(cellId, cellConfig);
    } else {
        throw std::bad_typeid();
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0]
                  << " SCENARIO_CONFIG.json [MAX_SIM_TIME]" << std::endl;
        return -1;
    }
    std::string configFilePath = argv[1];
    double simTime = (argc > 2) ? std::stod(argv[2]) : 100.0;

    // Build the grid-coupled model
    auto model = std::make_shared<
        GridCellDEVSCoupled<vegetationState, double>
    >("vegetation", addGridCell, configFilePath);
    model->buildModel();

    // Set up the coordinator and logger (logs every 1.0 time unit)
    RootCoordinator rootCoordinator(model);
    rootCoordinator.setLogger<CustomCSVLogger>("vegetation_log.csv", ";", 1.0);

    rootCoordinator.start();

    // Run simulation with a simple console progress bar
    const double interval = 1.0;
    double currentTime = 0.0;
    const int barWidth = 50;

    while (currentTime < simTime) {
        currentTime += interval;
        if (currentTime > simTime) currentTime = simTime;

        rootCoordinator.simulate(currentTime);

        double pct = (currentTime / simTime) * 100.0;
        int pos = static_cast<int>(barWidth * pct / 100.0);
        std::cout << "\rProgress: [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos)      std::cout << "=";
            else if (i == pos) std::cout << ">";
            else               std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1)
                  << pct << "%, t=" << currentTime << "/" << simTime;
        std::cout.flush();
    }
    std::cout << std::endl;

    rootCoordinator.stop();
    std::cout << "Simulation completed at t=" << currentTime << std::endl;
    return 0;
}
