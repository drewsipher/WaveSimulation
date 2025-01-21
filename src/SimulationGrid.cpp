#include "SimulationGrid.h"
#include <iostream>
#include <algorithm>

SimulationGrid::SimulationGrid(const int& width, const int& height): _width(width), _height(height)
{
    base = cv::Mat_<double>::zeros(height,width);
    density = cv::Mat_<double>::ones(height, width) * default_density;
    damping_mask = cv::Mat_<double>::ones(height,width);

    // Define damping layer properties
    CreateDampingMask();
}

// // Copy constructor
// SimulationGrid::SimulationGrid(const SimulationGrid& other)
//     : _width(other._width), _height(other._height), spacing(other.spacing),
//       damping_thickness(other.damping_thickness), default_density(other.default_density),
//       density_variations(other.density_variations), base(other.base.clone()),
//       density(other.density.clone()), damping_mask(other.damping_mask.clone()) {}

// // Copy assignment operator
// SimulationGrid& SimulationGrid::operator=(const SimulationGrid& other) {
//     if (this == &other) {
//         return *this;
//     }
//     _width = other._width;
//     _height = other._height;
//     spacing = other.spacing;
//     damping_thickness = other.damping_thickness;
//     default_density = other.default_density;
//     density_variations = other.density_variations;
//     base = other.base.clone();
//     density = other.density.clone();
//     damping_mask = other.damping_mask.clone();
//     return *this;
// }

void SimulationGrid::SetDensityArea(const DensityVariation& variation) {
    for (int i = variation.y_range.first; i < variation.y_range.second; ++i) {
        for (int j = variation.x_range.first; j < variation.x_range.second; ++j) {
            density(i, j) = variation.density;
        }
    }
}

void SimulationGrid::CreateDampingMask() {
    double damping_strength = 0.95;

    // Top and Bottom Damping
    for (int i = 0; i < damping_thickness; ++i) {
        double factor = damping_strength + (1.0 - damping_strength) * (i / static_cast<double>(damping_thickness));
        for (int j = 0; j < _width; ++j) {
            damping_mask(i,j) *= factor; // Top
            damping_mask(_width - i - 1, j) *= factor;  // Bottom
        }
    }

    // Left and Right Damping
    for (int i = 0; i < damping_thickness; ++i) {
        double factor = damping_strength + (1.0 - damping_strength) * (i / static_cast<double>(damping_thickness));
        for (int j = 0; j < _height; ++j) {
            damping_mask(j, i) *= factor;  // Left
            damping_mask(j, _height - i - 1) *= factor;  // Right
        }
    }
}