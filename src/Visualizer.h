#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <opencv2/opencv.hpp>
#include <GLFW/glfw3.h>
#include "imgui.h"

class Visualizer {
public:
    Visualizer(int width, int height);
    void show();
    void update();
    void close();
    bool isClosed() const;

private:
    bool closed;
    double min_value;
    double max_value;
    int _width;
    int _height;
    cv::Mat tint;
    cv::Mat cmap;
    std::vector<std::pair<int, int>> audio_sources;
    std::vector<std::pair<int, int>> barriers;

    void applyTint();
    void onMouse(int event, int x, int y, int flags);
    static void onMouseWrapper(int event, int x, int y, int flags, void* userdata);
    std::string loadShaderSource(const char* filePath);
    GLuint compileShader(GLenum type, const char* filePath);
    GLuint createShaderProgram();
    void createQuad(); 
    void createTextures();
    GLuint createComputeShader(const char* filePath);
    void checkOpenGLError(const std::string& errorMessage);


    

    GLFWwindow* window;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    GLuint shaderProgram;
    GLuint waveShaderProgram;
    GLuint VAO, VBO, EBO;
    GLuint framebufferTexture, currentWaveTex, previousWaveTex, nextWaveTex, speedTex;
    GLuint FBO;
    GLuint textureID;

};

#endif // VISUALIZER_H