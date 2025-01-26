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
    bool _closed = false;
    bool _simulationStart = false;
    int _width;
    int _height;

    std::string LoadShaderSource(const char* filePath);
    GLuint CompileShader(GLenum type, const char* filePath);
    GLuint CreateShaderProgram();
    void CreateQuad(); 
    void CreateTextures();
    GLuint CreateComputeShader(const char* filePath);
    void CheckOpenGLError(const std::string& errorMessage);
    void DrawUI();

    GLFWwindow* _window;
    GLuint _shaderProgram;
    GLuint _waveShaderProgram;
    GLuint _VAO, _VBO, _EBO;
    GLuint _framebufferTexture, _currentWaveTex, _previousWaveTex, _nextWaveTex, _speedsTex, _sourcesTex;
    GLuint _FBO;
    GLuint _textureID;

};

#endif // VISUALIZER_H