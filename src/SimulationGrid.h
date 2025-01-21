#ifndef GRID_H
#define GRID_H

#include <vector>
#include <opencv2/opencv.hpp>

class SimulationGrid {
public:
    struct DensityVariation {
        std::pair<int, int> x_range;
        std::pair<int, int> y_range;
        double density;
    };

    SimulationGrid(const int& width = 500, const int& height = 500);
    // SimulationGrid(const SimulationGrid& other); // Copy constructor
    // SimulationGrid& operator=(const SimulationGrid& other); // Copy assignment operator
    void SetDensityArea(const DensityVariation& variation);
    
    cv::Mat_<double> base;
    cv::Mat_<double> density;
    cv::Mat_<double> damping_mask;

    int Width() const { return _width; }
    int Height() const { return _height; }

    // Overload the () operator for non-const access
    double& operator()(int row, int col) {
        return base(row, col);
    }

    // Overload the () operator for const access
    const double& operator()(int row, int col) const {
        return base(row, col);
    }

private:
    void CreateDampingMask();

    int _width;
    int _height;
    double spacing = 0.01;
    int damping_thickness = 15;
    double default_density = 1.0;

    std::vector<DensityVariation> density_variations;
};

#endif // GRID_H