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
    ImGui_ImplOpenGL3_Init("#version 330");

    checkOpenGLError("GLEW Initialization");

    shaderProgram = createShaderProgram();
    waveShaderProgram = createComputeShader("../shaders/Wave.comp");
    checkOpenGLError("Shader Program Creation");


    createQuad();
    createTextures();
    checkOpenGLError("Rendering");
}

GLuint Visualizer::createComputeShader(const char* filePath) {
    std::string computeSource = loadShaderSource(filePath);
    GLuint computeShader = compileShader(GL_COMPUTE_SHADER, computeSource.c_str());

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

void Visualizer::createTextures() {
    // Create textures for current, previous, and next wave data
    glGenTextures(1, &currentWaveTex);
    glBindTexture(GL_TEXTURE_2D, currentWaveTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _physics.grid.Width(), _physics.grid.Height(), 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &previousWaveTex);
    glBindTexture(GL_TEXTURE_2D, previousWaveTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _physics.grid.Width(), _physics.grid.Height(), 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &nextWaveTex);
    glBindTexture(GL_TEXTURE_2D, nextWaveTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _physics.grid.Width(), _physics.grid.Height(), 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

GLuint Visualizer::compileShader(GLenum type, const char* source) {
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

void Visualizer::createQuad() {
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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

GLuint Visualizer::createShaderProgram() {
    std::string vertexSource = loadShaderSource("../shaders/SimpleVert.glsl");
    std::string fragmentSource = loadShaderSource("../shaders/SimpleFrag.glsl");

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

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

void Visualizer::update() {
    // Dispatch compute shader
    glUseProgram(waveShaderProgram);
    glBindImageTexture(0, currentWaveTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glBindImageTexture(1, previousWaveTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glBindImageTexture(2, nextWaveTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glDispatchCompute((GLuint)ceil(_physics.grid.Width() / 16.0), (GLuint)ceil(_physics.grid.Height() / 16.0), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Swap textures
    std::swap(currentWaveTex, previousWaveTex);
    std::swap(currentWaveTex, nextWaveTex);

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

    glUniform1i(glGetUniformLocation(shaderProgram, "currentWave"), 0);
    
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentWaveTex); // Bind the framebuffer texture

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
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
        glDeleteBuffers(1, &EBO);
        glDeleteTextures(1, &currentWaveTex);
        glDeleteTextures(1, &previousWaveTex);
        glDeleteTextures(1, &nextWaveTex);
        glDeleteProgram(shaderProgram);
        glDeleteProgram(waveShaderProgram);
    }
}

bool Visualizer::isClosed() const {
    return closed || glfwWindowShouldClose(window);
}

void Visualizer::checkOpenGLError(const std::string& errorMessage) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error (" << errorMessage << "): " << err << std::endl;
    }
}