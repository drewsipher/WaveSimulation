#include "Visualizer.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

Visualizer::Visualizer(Physics& physics) : _physics(physics), closed(false), min_value(-1.0), max_value(1.0) 
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create a GLFW window
    window = glfwCreateWindow(_physics.grid.Width(), _physics.grid.Height(), "Wave Simulation", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

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
    cv::normalize(wave_image, wave_image, 0, 255, cv::NORM_MINMAX);
    wave_image.convertTo(wave_image, CV_8UC1);
    cv::Mat wave_image_8UC3;
    cv::cvtColor(wave_image, wave_image_8UC3, cv::COLOR_GRAY2BGR);

    cv::applyColorMap(wave_image_8UC3, image, cv::COLORMAP_VIRIDIS);

    cv::Mat tinted_wave = cv::Mat::zeros(_physics.grid.Height(), _physics.grid.Width(), CV_8UC3);
    cv::multiply(image, tint, tinted_wave, 1.0 / 255.0);

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create ImGui window
    ImGui::Begin("Wave Simulation");
    ImGui::Text("This is some useful text.");
    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // cv::imshow("Wave Simulation", tinted_wave);
    // int key = cv::waitKey(1);
    // if (key == 27) {  // 27 is the ASCII code for the ESC key
    //     close();
    // } else if (key == 'p') {
    //     cv::waitKey(0);  // Pause the simulation until any key is pressed
    // }
}

void Visualizer::show() {
    cv::namedWindow("Wave Simulation");
    cv::setMouseCallback("Wave Simulation", onMouseWrapper, this);
    // Run simulation
    std::cout << "Starting simulation loop..." << std::endl;
    while (!isClosed()) {
        update();
    }
}

void Visualizer::close() {
    closed = true;
    cv::destroyWindow("Wave Simulation");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool Visualizer::isClosed() const {
    return closed;
}

void Visualizer::onMouse(int event, int x, int y, int) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        audio_sources.emplace_back(x, y);
        _physics.AddPing(x,y, 1.0);  // Example value for audio source
    } 
}

void Visualizer::onMouseWrapper(int event, int x, int y, int flags, void* userdata) {
    Visualizer* self = static_cast<Visualizer*>(userdata);
    self->onMouse(event, x, y, flags);
}