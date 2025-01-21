#include "Visualizer.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

Visualizer::Visualizer(Physics& physics) : _physics(physics), closed(false), min_value(-1.0), max_value(1.0) 
{
    image = cv::Mat::zeros(_physics.grid.Height(), _physics.grid.Width(), CV_8UC3);
    applyTint();
}

void Visualizer::applyTint() {
    tint = cv::Mat(_physics.grid.Height(), _physics.grid.Width(), CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < _physics.grid.Height(); ++i) {
        for (int j = 0; j < _physics.grid.Width(); ++j) {
            double density = _physics.grid.density[i][j];
            tint.at<cv::Vec3b>(i, j) = cv::Vec3b(255 * static_cast<int>(density), 255 * static_cast<int>(density), 255 * static_cast<int>(density));
        }
    }
}

void Visualizer::update() {
    
    _physics.update();

    cv::Mat wave_image = _physics.grid.base.clone();
    // cv::normalize(wave_image, wave_image, 0, 255, cv::NORM_MINMAX);
    wave_image.convertTo(wave_image, CV_8UC1);
    wave_image *= 255;
    cv::Mat wave_image_8UC3;
    cv::cvtColor(wave_image, wave_image_8UC3, cv::COLOR_GRAY2BGR);

    cv::applyColorMap(wave_image_8UC3, image, cv::COLORMAP_VIRIDIS);

    cv::Mat tinted_wave = cv::Mat::zeros(_physics.grid.Height(), _physics.grid.Width(), CV_8UC3);
    cv::multiply(image, tint, tinted_wave, 1.0 / 255.0);

    cv::imshow("Wave Simulation", tinted_wave);
    int key = cv::waitKey(1);
    if (key == 27) {  // 27 is the ASCII code for the ESC key
        close();
    }
}

void Visualizer::show() {
    cv::namedWindow("Wave Simulation");
    cv::setMouseCallback("Wave Simulation", onMouseWrapper, this);
}

void Visualizer::close() {
    closed = true;
    cv::destroyWindow("Wave Simulation");
}

bool Visualizer::isClosed() const {
    return closed;
}

void Visualizer::onMouse(int event, int x, int y, int) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        audio_sources.emplace_back(x, y);
        _physics.AddPing(x,y, 10.0);  // Example value for audio source
    } else if (event == cv::EVENT_RBUTTONDOWN) {
        barriers.emplace_back(x, y);
        _physics.AddPing(y, x, -1.0);  // Example value for barrier
    }
}

void Visualizer::onMouseWrapper(int event, int x, int y, int flags, void* userdata) {
    Visualizer* self = static_cast<Visualizer*>(userdata);
    self->onMouse(event, x, y, flags);
}