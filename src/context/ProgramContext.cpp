#include "ProgramContext.h"
#include "RenderingSystem.h"
#include "InputSystem.h"
#include <iostream>
#include <memory>

ProgramContext::ProgramContext()
{
    glfwSetErrorCallback( (GLFWerrorfun)errorCallback );
}

ProgramContext::~ProgramContext()
{
    glfwTerminate();
}

void ProgramContext::errorCallback(int error, const char * description)
{
    std::cout << description;
}

void ProgramContext::init()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        abort();
    }
    ProgramVariables::init();
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK)
    {
        std::cout << "Error initializing GLEW: " << glewGetErrorString(glewErr) << std::endl;
    }
    sceneManager_ = SceneManager::createInstance();
    sceneManager_->activateScene("default");

    systems_.push_back(std::make_unique<RenderingSystem>());
    systems_.push_back(std::make_unique<InputSystem>());
}

void ProgramContext::run()
{
    do
    {
        static GLdouble lastTime = glfwGetTime();
        static GLdouble fpsLastTime = 0;
        GLdouble currentTime = glfwGetTime();
        ProgramVariables::setDeltaTime(GLfloat(currentTime - lastTime));

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update the systems
        for (auto& system : systems_) system->update(ProgramVariables::getDeltaTime());

        ++FPScount_;
        if (currentTime - fpsLastTime >= 1)
        {
            glfwSetWindowTitle(ProgramVariables::getWindow(), std::to_string(FPScount_).c_str());
            FPScount_ = 0;
            fpsLastTime = currentTime;
        }
        lastTime = currentTime;

        glfwSwapBuffers(ProgramVariables::getWindow());
        glfwPollEvents();
    } while (!glfwWindowShouldClose(ProgramVariables::getWindow()));
}