#include <iostream>
#include <opencv2/opencv.hpp>
// #include "src/WaveSimulation.h"
// #include "src/SimulationGrid.h"
// #include "src/Visualizer.h"

int main() {
    cv::imshow("Hello, World!", cv::Mat::zeros(100, 100, CV_8UC1));
    cv::waitKey(0);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
// 

// int main() {
//     // Initialize simulation grid
//     std::cout << "Initializing simulation grid..." << std::endl;
//     SimulationGrid grid = SimulationGrid();
//     std::cout << "Initial grid values:" << std::endl;
//     grid.printGrid();

//     // Initialize wave simulation
//     std::cout << "Initializing wave simulation..." << std::endl;
//     WaveSimulation simulation(grid);

//     // Initialize visualizer
//     std::cout << "Initializing visualizer..." << std::endl;
//     Visualizer visualizer(grid);
//     visualizer.show();

//     // Run simulation
//     std::cout << "Starting simulation loop..." << std::endl;
//     while (!visualizer.isClosed()) {
//         simulation.update();
//         visualizer.update();
//     }

//     visualizer.close();
//     std::cout << "Simulation ended." << std::endl;

//     return 0;
// }