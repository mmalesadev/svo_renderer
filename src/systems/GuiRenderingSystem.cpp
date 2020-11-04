#include "GuiRenderingSystem.h"
#include "ProgramVariables.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GuiRenderingSystem::GuiRenderingSystem(std::vector< std::pair<std::string, std::string> >& actionList)
    : actionList_(actionList)
{
    const char* glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(ProgramVariables::getWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    windowSize_ = ProgramVariables::getWindowSize();
}

GuiRenderingSystem::~GuiRenderingSystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiRenderingSystem::update()
{
    render();
}

void GuiRenderingSystem::render()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Begin("Menu");
    if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::Text("Entities");
        ImGui::Columns(2);
        ImGui::Separator();
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Separator();
        for (auto& entity : entities)
        {
            ImGui::Text("%d", entity->getId());
            ImGui::NextColumn();
            ImGui::Text("%s", entity->getName().c_str());
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();
    }
    if (ImGui::CollapsingHeader("Splat Manipulation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Demo Window", &show_demo_window);
    }
    if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Columns(2, "controls_columns"); // 4-ways, with border
        ImGui::Separator();
        ImGui::Text("Action"); ImGui::NextColumn();
        ImGui::Text("Key"); ImGui::NextColumn();
        ImGui::Separator();
        for (auto& action : actionList_)
        {
            ImGui::Text("%s", action.first.c_str());
            ImGui::NextColumn();
            ImGui::Text("%s", action.second.c_str());
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();
    }
    if (ImGui::CollapsingHeader("About", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::TextWrapped(
            "This program is a sparse voxel octree renderer." \
            "It passes octree leaves positions (3d points) to the GPU and uses geometry shader to tranform them into splats (squres, which are always facing the active camera)."
            );
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
