#include "ProgramContext.h"
#include "RenderingSystem.h"
#include "GuiSystem.h"
#include "InputSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

    GLFWimage iconImages[1];
    iconImages[0].pixels = stbi_load("../data/icons/svo_renderer_icon.png", &iconImages[0].width, &iconImages[0].height, 0, 4);
    glfwSetWindowIcon(window_, 1, iconImages);
    stbi_image_free(iconImages[0].pixels);

    sceneManager_ = SceneManager::createInstance();
    sceneManager_->activateScene("default");
}

void ProgramContext::run()
{
    while (!glfwWindowShouldClose(window_))
    {
        static GLdouble lastTime = glfwGetTime();
        GLdouble currentTime = glfwGetTime();
        ProgramVariables::setDeltaTime(GLfloat(currentTime - lastTime));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        sceneManager_->updateSystems();

        lastTime = currentTime;

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}