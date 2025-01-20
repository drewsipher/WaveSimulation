#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include "SimulationGrid.h"

class WaveSimulation {
public:
    WaveSimulation(SimulationGrid& grid);
    void update();

private:
    void computeLaplacian();

    SimulationGrid& grid;
    std::vector<std::vector<double>> new_grid;
    double base_speed = 343.0;
    double time_step = 0.0005;
    double damping = 0.9999;
    std::vector<std::vector<double>> speed;
    std::vector<std::vector<double>> coeff;
    std::vector<std::vector<double>> laplacian;
};

#endif // PHYSICS_H