#include "ProgramContext.h"
#include "WorldRenderingSystem.h"
#include "GuiRenderingSystem.h"
#include "InputSystem.h"

#include <iostream>
#include <memory>

ProgramContext::ProgramContext() : window_(nullptr)
{
    glfwSetErrorCallback( (GLFWerrorfun)errorCallback );
}

ProgramContext::~ProgramContext()
{
    glfwDestroyWindow(ProgramVariables::getWindow());
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
    window_ = ProgramVariables::getWindow();
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK)
    {
        std::cout << "Error initializing GLEW: " << glewGetErrorString(glewErr) << std::endl;
    }

    sceneManager_ = SceneManager::createInstance();
    sceneManager_->activateScene("default");

    auto inputSystem = std::make_unique<InputSystem>();
    auto worldRenderingSystem = std::make_unique<WorldRenderingSystem>();
    auto guiRenderingSystem = std::make_unique<GuiRenderingSystem>(inputSystem->getActionList());
    systems_.push_back(std::move(inputSystem));
    systems_.push_back(std::move(worldRenderingSystem));
    systems_.push_back(std::move(guiRenderingSystem));
}

void ProgramContext::run()
{
    while (!glfwWindowShouldClose(window_))
    {
        static GLdouble lastTime = glfwGetTime();
        static GLdouble fpsLastTime = 0;
        GLdouble currentTime = glfwGetTime();
        ProgramVariables::setDeltaTime(GLfloat(currentTime - lastTime));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Update the systems
        for (auto& system : systems_) system->update();

        ++FPScount_;
        if (currentTime - fpsLastTime >= 1)
        {
            glfwSetWindowTitle(window_, std::to_string(FPScount_).c_str());
            FPScount_ = 0;
            fpsLastTime = currentTime;
        }
        lastTime = currentTime;

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}