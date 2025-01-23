#include "Physics.h"
#include <vector>
#include <algorithm>
#include <opencv2/imgproc.hpp>

Physics::Physics(const int& width, const int& height) : grid(width, height)
{    
    _speed = cv::Mat_<double>::zeros(grid.Height(), grid.Width());
    _coeff = cv::Mat_<double>::zeros(grid.Height(), grid.Width());
    _laplacian = cv::Mat_<double>::zeros(grid.Height(), grid.Width());
    _tempGridVals = cv::Mat_<double>::zeros(grid.Height(), grid.Width());
    _previousGrid = cv::Mat_<double>::zeros(grid.Height(), grid.Width());

    // Calculate speed grid based on density (e.g., inverse relation)
    for (int i = 0; i < grid.Height(); ++i) {
        for (int j = 0; j < grid.Width(); ++j) {
            _speed(i,j) = base_speed / grid.density(i,j);
            _coeff(i,j) = (_speed(i,j) * _speed(i,j)) * (time_step * time_step);
        }
    }
}

void Physics::AddPing(int x, int y, double amplitude) {
    grid(y, x) = amplitude;
}


void Physics::update() {
    // Compute Laplacian without wrapping
    computeLaplacian();

    // Wave equation update
    for (int i = 0; i < grid.Height(); ++i) {
        for (int j = 0; j < grid.Width(); ++j) {
            _tempGridVals(i,j) = 2 * grid(i,j) - _previousGrid(i,j) + _coeff(i,j) * _laplacian(i,j);
            _tempGridVals(i,j) *= grid.damping_mask(i,j);
        }
    }

    // Update grids
    _previousGrid  = grid.base.clone();
    grid.base = _tempGridVals.clone();
}
void Physics::computeLaplacian() {
    cv::Laplacian(grid.base, _laplacian, CV_64F, 1, 1, 0, cv::BORDER_ISOLATED);
}