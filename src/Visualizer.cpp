#include <GL/glew.h>
#include "Visualizer.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

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

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    image = cv::Mat::zeros(_physics.grid.Height(), _physics.grid.Width(), CV_8UC3);
    applyTint();


    shaderProgram = createShaderProgram();
    createQuad();
}

std::string Visualizer::loadShaderSource(const char* filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}

GLuint Visualizer::compileShader(GLenum type, const char* filePath) {
    std::string shaderSource = loadShaderSource(filePath);
    const char* source = shaderSource.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

void Visualizer::createQuad() {
    float vertices[] = {
        // Positions
        -1.0f,  1.0f, // Top-left
         1.0f,  1.0f, // Top-right
         1.0f, -1.0f, // Bottom-right
        -1.0f, -1.0f  // Bottom-left
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint Visualizer::createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "../shaders/SimpleVert.glsl");
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "../shaders/SimpleFrag.glsl");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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

    // Use shader program and draw quad
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void Visualizer::show() {
    // Run simulation
    std::cout << "Starting simulation loop..." << std::endl;
    while (!isClosed()) {
        update();
        glfwPollEvents(); // Poll for and process events
    }
    close(); // Ensure resources are cleaned up when the loop exits
}

void Visualizer::close() {
    if (!closed) {
        closed = true;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();

        // Cleanup OpenGL resources
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
}

bool Visualizer::isClosed() const {
    return closed || glfwWindowShouldClose(window);
}