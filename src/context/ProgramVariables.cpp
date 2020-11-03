#include "ProgramVariables.h"

GLFWwindow * ProgramVariables::window_;
float ProgramVariables::deltaTime_;

void ProgramVariables::init()
{
    GLFWmonitor * monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * desktop = glfwGetVideoMode(monitor);
    window_ = glfwCreateWindow(desktop->width, desktop->height, "svo-renderer", nullptr, nullptr);

    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwMakeContextCurrent(window_);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwSetCursorPos(window_, desktop->width / 2, desktop->height / 2);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSwapInterval(0);    // Disable vsync
}

GLFWwindow * ProgramVariables::getWindow()
{
    return window_;
}

void ProgramVariables::setDeltaTime(float deltaTime)
{
    deltaTime_ = deltaTime;
}

float ProgramVariables::getDeltaTime()
{
    return deltaTime_;
}

std::pair<int, int> ProgramVariables::getWindowSize()
{
    int windowWidth, windowHeight;
    glfwGetWindowSize(window_, &windowWidth, &windowHeight);
    return std::pair<int, int>(windowWidth, windowHeight);
}
