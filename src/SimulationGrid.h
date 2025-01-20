#ifndef GRID_H
#define GRID_H

#include <vector>

class SimulationGrid {
public:
    struct DensityVariation {
        std::pair<int, int> x_range;
        std::pair<int, int> y_range;
        double density;
    };

    SimulationGrid(const int& size = 500);
    void update();
    void printGrid() const;
    void setDensityArea(const DensityVariation& variation);

    int size() const { return grid_size; }

    std::vector<std::vector<double>> grid;
    std::vector<std::vector<double>> prev_grid;
    std::vector<std::vector<double>> density;
    std::vector<std::vector<double>> damping_mask;

    


private:
    // void setDensityAreaImpl(const DensityVariation& variation);
    void createDampingMask();

    int grid_size;
    double spacing = 0.01;
    int damping_thickness = 25;
    double default_density = 1.0;

    std::vector<DensityVariation> density_variations;
};

#endif // GRID_H