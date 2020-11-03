#include "ProgramContext.h"
#include "RenderingSystem.h"
#include "InputSystem.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <memory>

ProgramContext::ProgramContext() : window_(nullptr)
{
    glfwSetErrorCallback( (GLFWerrorfun)errorCallback );
}

ProgramContext::~ProgramContext()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

    const char* glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    sceneManager_ = SceneManager::createInstance();
    sceneManager_->activateScene("default");

    systems_.push_back(std::make_unique<RenderingSystem>());
    systems_.push_back(std::make_unique<InputSystem>());
}

void ProgramContext::run()
{
    bool show_demo_window = true;

    while (!glfwWindowShouldClose(window_))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        static GLdouble lastTime = glfwGetTime();
        static GLdouble fpsLastTime = 0;
        GLdouble currentTime = glfwGetTime();
        ProgramVariables::setDeltaTime(GLfloat(currentTime - lastTime));

        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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