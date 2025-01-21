#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include "SimulationGrid.h"
#include <opencv2/opencv.hpp>

class Physics {
public:
    Physics(const int& width, const int& height);
    void update();
    void AddPing(int x, int y, double amplitude);
    SimulationGrid grid;

private:
    void computeLaplacian();

    double base_speed = 343.0;
    double time_step = 0.0005;
    double damping = 0.9999;
    cv::Mat_<double> _speed;
    cv::Mat_<double> _coeff;
    cv::Mat_<double> _laplacian;
    cv::Mat_<double> _tempGridVals;
    cv::Mat_<double> _previousGrid;
};

#endif // PHYSICS_H