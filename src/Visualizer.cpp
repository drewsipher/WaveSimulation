#include "Visualizer.h"
#include <opencv2/opencv.hpp>
#include <iostream>

Visualizer::Visualizer(SimulationGrid& grid)
    : grid(grid), closed(false), min_value(-1.0), max_value(1.0) {
    image = cv::Mat::zeros(grid.size(), grid.size(), CV_8UC3);
    applyTint();
}

void Visualizer::applyTint() {
    tint = cv::Mat(grid.size(), grid.size(), CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid.size(); ++j) {
            double density = grid.density[i][j];
            tint.at<cv::Vec3b>(i, j) = cv::Vec3b(255 * static_cast<int>(density), 255 * static_cast<int>(density), 255 * static_cast<int>(density));
        }
    }
}

void Visualizer::update() {
    cv::Mat wave_image = cv::Mat(grid.size(), grid.size(), CV_64F, grid.grid.data());
    cv::normalize(wave_image, wave_image, 0, 1, cv::NORM_MINMAX);
    cv::applyColorMap(wave_image * 255, image, cv::COLORMAP_VIRIDIS);

    cv::Mat tinted_wave;
    cv::multiply(image, tint, tinted_wave, 1.0 / 255.0);

    cv::imshow("Wave Simulation", tinted_wave);
    cv::waitKey(1);
}

void Visualizer::show() {
    cv::namedWindow("Wave Simulation");
    cv::setMouseCallback("Wave Simulation", onMouseWrapper, this);
    while (!closed) {
        update();
    }
}

void Visualizer::close() {
    closed = true;
    cv::destroyWindow("Wave Simulation");
}

bool Visualizer::isClosed() const {
    return closed;
}

void Visualizer::onMouse(int event, int x, int y, int flags) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        audio_sources.emplace_back(x, y);
        grid.grid[y][x] = 1;  // Example value for audio source
    } else if (event == cv::EVENT_RBUTTONDOWN) {
        barriers.emplace_back(x, y);
        grid.grid[y][x] = -1;  // Example value for barrier
    }
}

void Visualizer::onMouseWrapper(int event, int x, int y, int flags, void* userdata) {
    Visualizer* self = static_cast<Visualizer*>(userdata);
    self->onMouse(event, x, y, flags);
}