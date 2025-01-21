#include <iostream>
#include "src/Physics.h"
#include "src/SimulationGrid.h"
#include "src/Visualizer.h"

int main() {
    // Initialize wave simulation
    std::cout << "Initializing wave simulation..." << std::endl;
    Physics simulation(300, 300);

    // Initialize visualizer
    std::cout << "Initializing visualizer..." << std::endl;
    Visualizer visualizer = Visualizer(simulation);
    visualizer.show();
    
    // Run simulation
    std::cout << "Starting simulation loop..." << std::endl;
    while (!visualizer.isClosed()) {
        visualizer.update();
        // simulation.update();
    }

    visualizer.close();
    std::cout << "Simulation ended." << std::endl;

    return 0;
}