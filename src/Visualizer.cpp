#include <GL/glew.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "Visualizer.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

Visualizer::Visualizer(int width, int height) : _width(width), _height(height)
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create a GLFW window
    _window = glfwCreateWindow(_width, _height, "Wave Simulation", NULL, NULL);
    if (_window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(_window);
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
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    CheckOpenGLError("GLEW Initialization");

    _shaderProgram = CreateShaderProgram();
    _waveShaderProgram = CreateComputeShader("../shaders/Wave.comp");
    CheckOpenGLError("Shader Program Creation");

    CreateQuad();
    CreateTextures();
    CheckOpenGLError("Rendering");
}

GLuint Visualizer::CreateComputeShader(const char* filePath) {
    std::string computeSource = LoadShaderSource(filePath);
    GLuint computeShader = CompileShader(GL_COMPUTE_SHADER, computeSource.c_str());

    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(computeShader);

    return program;
}

void Visualizer::CreateTextures() {
    // Create textures for current, previous, and next wave data

    glGenTextures(1, &_currentWaveTex);
    glBindTexture(GL_TEXTURE_2D, _currentWaveTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _width, _height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &_previousWaveTex);
    glBindTexture(GL_TEXTURE_2D, _previousWaveTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _width, _height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &_nextWaveTex);
    glBindTexture(GL_TEXTURE_2D, _nextWaveTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _width, _height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create texture for density data
    cv::Mat speedImage = cv::Mat::zeros(_height, _width, CV_32F);
    speedImage.setTo(343.0f);
    _maxSimSpeed = 343.0f;
    // cv::circle(speedImage, cv::Point(_width / 2, _height / 2), std::min(_width, _height) / 4, cv::Scalar(200.0f), -1);
    
    glGenTextures(1, &_speedsTex);
    glBindTexture(GL_TEXTURE_2D, _speedsTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _width, _height, 0, GL_RED, GL_FLOAT, speedImage.ptr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &_sourcesTex);
    glBindTexture(GL_TEXTURE_2D, _sourcesTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

std::string Visualizer::LoadShaderSource(const char* filePath) {
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

GLuint Visualizer::CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

void Visualizer::CreateQuad() {
    float vertices[] = {
        // Positions
        -1.0f,  1.0f, // Top-left
         1.0f,  1.0f, // Top-right
         1.0f, -1.0f, // Bottom-right
        -1.0f, -1.0f  // Bottom-left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

GLuint Visualizer::CreateShaderProgram() {
    std::string vertexSource = LoadShaderSource("../shaders/SimpleVert.glsl");
    std::string fragmentSource = LoadShaderSource("../shaders/SimpleFrag.glsl");

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource.c_str());
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Visualizer::DrawUI() {
    ImGui::Begin("Wave Simulation");
    if (_simulationStart)
    {
        if (ImGui::Button("Stop")) {
            std::cout << "Stopping" << std::endl;
            _simulationStart = false;
        } 
        ImGui::BeginDisabled();
        ImGui::Button("Reset");
        ImGui::EndDisabled();
    } else 
    {
        if (ImGui::Button("Start")) {
            std::cout << "Starting" << std::endl;
            _simulationStart = true;
        }
        if (ImGui::Button("Reset")) {
            Reset();
            std::cout << "Reset button pressed" << std::endl;
        }
    }

    ImGui::End();
    ImGui::Begin("Sources");

    // static float speed = 343.0f;
    // ImGui::SliderFloat("Speed", &speed, 0.0f, 1000.0f);

    ImGui::SliderFloat("Frequency", &_frequency, 1.0f, 10000.0f);
    ImGui::SliderFloat("Length", &_length, 1.0f, 10000.0f);
    ImGui::SliderFloat("Amplitude", &_amplitude, 0.0f, 100.0f);


    ImGui::End();
}

void Visualizer::update() {
    // Skip clicks if ImGui has focus
    if (!ImGui::GetIO().WantCaptureMouse) {
        if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(_window, &xpos, &ypos);
            AddValueToTexture(xpos, ypos);
        }
    }

    if (_simulationStart)
    {
        for (int i =0; i < 1;++i)
        {
        // Dispatch compute shader
        glUseProgram(_waveShaderProgram);
        glUniform1f(glGetUniformLocation(_waveShaderProgram, "maxSimSpeed"), _maxSimSpeed);
        glBindImageTexture(0, _currentWaveTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        glBindImageTexture(1, _previousWaveTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glBindImageTexture(2, _nextWaveTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glBindImageTexture(3, _speedsTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glBindImageTexture(4, _sourcesTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        glDispatchCompute((GLuint)ceil(_width / 16.0), (GLuint)ceil(_height / 16.0), 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Swap textures
        std::swap(_currentWaveTex, _previousWaveTex);
        std::swap(_currentWaveTex, _nextWaveTex);
        }
    }

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawUI();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use shader program and draw quad
    glUseProgram(_shaderProgram);

    glUniform1i(glGetUniformLocation(_shaderProgram, "currentWave"), 0);
    glUniform1i(glGetUniformLocation(_shaderProgram, "speedValues"), 1);
    glUniform1i(glGetUniformLocation(_shaderProgram, "sources"), 2);

    glBindVertexArray(_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _currentWaveTex); // Bind the framebuffer texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _speedsTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _sourcesTex);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void Visualizer::AddValueToTexture(double xpos, double ypos) {
    
    // Convert window coordinates to texture coordinates
    int tex_x = static_cast<int>(xpos);
    int tex_y = static_cast<int>(_height - ypos);

    if (_lastMousePoint == cv::Point2i(tex_x, tex_y)) {
        return;
    }
    _lastMousePoint = cv::Point2i(tex_x, tex_y);

    // Bind the sources texture
    glBindTexture(GL_TEXTURE_2D, _sourcesTex);

    // Read the current texture data
    std::vector<float> data(_width * _height * 4);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data.data());

    // Modify the texture data at the clicked position
    int index = (tex_y * _width + tex_x) * 4;
    data[index] = _length; // denotes the source is active
    data[index + 1] = _frequency; // the frequency of the source
    data[index + 2] = 0; // the length of the source
    data[index + 3] = _amplitude; // the amplitude of the source

    // Update the texture with the modified data
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGBA, GL_FLOAT, data.data());

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

}

void Visualizer::Reset() {

    std::vector<float> data(_width * _height * 4);
    std::fill(data.begin(), data.end(), 0.0f);
    
    // Bind the sources texture
    glBindTexture(GL_TEXTURE_2D, _sourcesTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGBA, GL_FLOAT, data.data());

    glBindTexture(GL_TEXTURE_2D, _currentWaveTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED, GL_FLOAT, data.data());

    glBindTexture(GL_TEXTURE_2D, _previousWaveTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED, GL_FLOAT, data.data());

    glBindTexture(GL_TEXTURE_2D, _nextWaveTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED, GL_FLOAT, data.data());

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

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
    if (!_closed) {
        _closed = true;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(_window);
        glfwTerminate();

        // Cleanup OpenGL resources
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
        glDeleteBuffers(1, &_EBO);
        glDeleteTextures(1, &_currentWaveTex);
        glDeleteTextures(1, &_previousWaveTex);
        glDeleteTextures(1, &_nextWaveTex);
        glDeleteTextures(1, &_speedsTex);
        glDeleteTextures(1, &_sourcesTex);
        glDeleteProgram(_shaderProgram);
        glDeleteProgram(_waveShaderProgram);
    }
}

bool Visualizer::isClosed() const {
    return _closed || glfwWindowShouldClose(_window);
}

void Visualizer::CheckOpenGLError(const std::string& errorMessage) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error (" << errorMessage << "): " << err << std::endl;
    }
}