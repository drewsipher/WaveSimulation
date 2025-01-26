#include <iostream>
#include "src/SimulationGrid.h"
#include "src/Visualizer.h"

int main() {
    // Initialize wave simulation
    std::cout << "Initializing wave simulation..." << std::endl;

    // Initialize visualizer
    std::cout << "Initializing visualizer..." << std::endl;
    Visualizer visualizer = Visualizer(600,600);
    visualizer.show();

    visualizer.close();
    std::cout << "Simulation ended." << std::endl;

    return 0;
}