#include "WaveSimulation.h"
#include <vector>
#include <algorithm>

WaveSimulation::WaveSimulation(SimulationGrid& GridSettings)
    : grid(grid) {
    new_grid.resize(grid.size(), std::vector<double>(grid.size(), 0.0));
    speed.resize(grid.size(), std::vector<double>(grid.size(), base_speed));
    coeff.resize(grid.size(), std::vector<double>(grid.size(), 0.0));
    laplacian.resize(grid.size(), std::vector<double>(grid.size(), 0.0));

    // Calculate speed grid based on density (e.g., inverse relation)
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid.size(); ++j) {
            speed[i][j] = base_speed / grid.density[i][j];
            coeff[i][j] = (speed[i][j] * speed[i][j]) * (time_step * time_step);
        }
    }
}

void WaveSimulation::update() {
    // Compute Laplacian without wrapping
    computeLaplacian();

    // Wave equation update
    for (int i = 1; i < grid.size() - 1; ++i) {
        for (int j = 1; j < grid.size() - 1; ++j) {
            new_grid[i][j] = 2 * grid.grid[i][j] - grid.prev_grid[i][j] + coeff[i][j] * laplacian[i][j];
            new_grid[i][j] *= damping;
            new_grid[i][j] *= grid.damping_mask[i][j];
        }
    }

    // Update grids
    std::copy(grid.grid.begin(), grid.grid.end(), grid.prev_grid.begin());
    std::copy(new_grid.begin(), new_grid.end(), grid.grid.begin());
}

void WaveSimulation::computeLaplacian() {
    for (int i = 1; i < grid.size() - 1; ++i) {
        for (int j = 1; j < grid.size() - 1; ++j) {
            laplacian[i][j] = (
                grid.grid[i - 1][j] + grid.grid[i + 1][j] +
                grid.grid[i][j - 1] + grid.grid[i][j + 1] -
                4 * grid.grid[i][j]
            );
        }
    }
}