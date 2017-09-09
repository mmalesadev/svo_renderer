#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <System.h>
#include "SceneManager.h"

class ProgramContext
{
private:
    static GLFWwindow * window_;
    std::unique_ptr<SceneManager> sceneManager_;

    std::string windowTitle_;
    int FPScount_ = 0;

    std::vector<std::unique_ptr<System>> systems_;
    
    static void errorCallback(int error, const char * description);

public:
    void init();
    void run();

    static GLFWwindow * getWindow();

    ProgramContext();
    ~ProgramContext();
};

