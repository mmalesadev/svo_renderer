#pragma once

#include <GLFW/glfw3.h>
#include <utility>

class ProgramVariables
{
private:
    static GLFWwindow * window_;
    static float deltaTime_;
    static bool guiVisible_;

public:
    static void init();

    static void toggleGui() { guiVisible_ = !guiVisible_; }
    static bool isGuiVisible() { return guiVisible_; }

    static GLFWwindow* getWindow();
    static void setDeltaTime(float deltaTime);
    static float getDeltaTime();
    static std::pair<int, int> getWindowSize();
};