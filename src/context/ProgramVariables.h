#pragma once

#include <GLFW/glfw3.h>
#include <utility>

class ProgramVariables
{
private:
    static GLFWwindow * window_;
    static float deltaTime_;

public:
    static void init();

    static GLFWwindow* getWindow();
    static void setDeltaTime(float deltaTime);
    static float getDeltaTime();
    static std::pair<int, int> getWindowSize();
};