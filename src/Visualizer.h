#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <opencv2/opencv.hpp>
#include "Physics.h"

class Visualizer {
public:
    Visualizer(Physics& physics);
    void show();
    void update();
    void close();
    bool isClosed() const;

private:
    Physics& _physics;
    cv::Mat image;
    bool closed;
    double min_value;
    double max_value;
    cv::Mat tint;
    cv::Mat cmap;
    std::vector<std::pair<int, int>> audio_sources;
    std::vector<std::pair<int, int>> barriers;

    void applyTint();
    void onMouse(int event, int x, int y, int flags);
    static void onMouseWrapper(int event, int x, int y, int flags, void* userdata);
};

#endif // VISUALIZER_H