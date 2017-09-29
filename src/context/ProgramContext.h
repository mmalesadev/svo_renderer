#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <System.h>
#include "SceneManager.h"
#include "ProgramVariables.h"

class ProgramContext
{
private:
    std::unique_ptr<SceneManager> sceneManager_;

    int FPScount_ = 0;

    std::vector<std::unique_ptr<System>> systems_;
    
    static void errorCallback(int error, const char * description);

public:
    void init();
    void run();

    ProgramContext();
    ~ProgramContext();
};

