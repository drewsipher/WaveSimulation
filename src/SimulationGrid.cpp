#include "SimulationGrid.h"
#include <iostream>
#include <algorithm>

SimulationGrid::SimulationGrid(const int& size)
    : grid_size(size) {
    grid.resize(grid_size, std::vector<double>(grid_size, 0.0));
    prev_grid.resize(grid_size, std::vector<double>(grid_size, 0.0));

    // Initialize density grid
    density.resize(grid_size, std::vector<double>(grid_size, default_density));

    // Define damping layer properties
    createDampingMask();
}

void SimulationGrid::setDensityArea(const DensityVariation& variation) {
    for (int i = variation.y_range.first; i < variation.y_range.second; ++i) {
        for (int j = variation.x_range.first; j < variation.x_range.second; ++j) {
            density[i][j] = variation.density;
        }
    }
}

void SimulationGrid::update() {
    std::copy(grid.begin(), grid.end(), prev_grid.begin());
}

void SimulationGrid::createDampingMask() {
    damping_mask.resize(grid_size, std::vector<double>(grid_size, 1.0));
    double damping_strength = 0.9;

    // Top and Bottom Damping
    for (int i = 0; i < damping_thickness; ++i) {
        double factor = damping_strength + (1.0 - damping_strength) * (i / static_cast<double>(damping_thickness));
        for (int j = 0; j < grid_size; ++j) {
            damping_mask[i][j] *= factor;  // Top
            damping_mask[grid_size - i - 1][j] *= factor;  // Bottom
        }
    }

    // Left and Right Damping
    for (int i = 0; i < damping_thickness; ++i) {
        double factor = damping_strength + (1.0 - damping_strength) * (i / static_cast<double>(damping_thickness));
        for (int j = 0; j < grid_size; ++j) {
            damping_mask[j][i] *= factor;  // Left
            damping_mask[j][grid_size - i - 1] *= factor;  // Right
        }
    }
}

void SimulationGrid::printGrid() const {
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}