#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GuiSystem.h"
#include "ProgramVariables.h"
#include "GraphicsComponent.h"

#include "SceneManager.h"

#include <memory>
#include <iostream>

GuiSystem::GuiSystem(std::vector< std::pair<std::string, std::string> >& actionList)
    : actionList_(actionList)
{
    const char* glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsCustom();
    ImGui_ImplGlfw_InitForOpenGL(ProgramVariables::getWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    windowSize_ = ProgramVariables::getWindowSize();

    for (auto& it : SceneManager::getInstance()->getEntityTypes()) {
        if (!it.second.graphicsComponent.empty()) {
            entityTypesStr_ += (it.first + '\0');
            entityTypes_.push_back(it.first);
        }
    }
}

GuiSystem::~GuiSystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiSystem::update()
{
    render();
}

void GuiSystem::render()
{
    if (!ProgramVariables::isGuiVisible()) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto renderingSystem = SceneManager::getInstance()->getRenderingSystem();

    if (show_demo_window_)
        ImGui::ShowDemoWindow(&show_demo_window_);

    ImGui::Begin("Menu", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
        int nVisibleEntities = 0;
        for (auto& entity : entities_)
        {
            auto& graphicsComponent = SceneManager::getInstance()->getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);
            if (graphicsComponent.isVisible()) nVisibleEntities += 1;
        }
        ImGui::Text("Visible entities: %d", nVisibleEntities);
        ImGui::Text("Total voxels: %lld", renderingSystem->getTotalVoxelsNumber());
        if (ImGui::Button("Save"))
        {
            SceneManager::getInstance()->saveActiveScene();
        }
    }
    if (ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen)) {
        static int chosen_entity_type_idx = -1;
        ImGui::Combo("##added_entity", &chosen_entity_type_idx, entityTypesStr_.c_str());
        if (ImGui::Button("Add entity")) {
            if (chosen_entity_type_idx >= 0)
                SceneManager::getInstance()->addNewEntity(entityTypes_[chosen_entity_type_idx]);
        }
    }
    if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::Text("Entities");
        ImGui::Columns(8);
        ImGui::Separator();
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Text("Color"); ImGui::NextColumn();
        ImGui::Text("Scale"); ImGui::NextColumn();
        ImGui::Text("x"); ImGui::NextColumn();
        ImGui::Text("y"); ImGui::NextColumn();
        ImGui::Text("z"); ImGui::NextColumn();
        ImGui::NextColumn();
        ImGui::Separator();
        for (auto& entity : entities_)
        {
            ImGui::Text("%d", entity);
            ImGui::NextColumn();
            auto& graphicsComponent = SceneManager::getInstance()->getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);
            auto& transformComponent = SceneManager::getInstance()->getComponent<TransformComponent>(entity, TRANSFORM_COMPONENT_ID);
            ImGui::Text("%s", graphicsComponent.getName().c_str());
            ImGui::NextColumn();
            renderEntityColorPicker(entity);
            ImGui::NextColumn();
            static ImGuiSliderFlags flags = ImGuiSliderFlags_None;
            float scale = transformComponent.getScale();
            if (ImGui::DragFloat(std::string("##scale" + std::to_string(entity)).c_str(), &scale, 1.0f, 1, 100, "%.0f", flags)) {
                transformComponent.setScale(scale);
            }
            ImGui::NextColumn();        
            glm::vec3 entityPosition = transformComponent.getPosition();
            if (ImGui::DragFloat(std::string("##x" + std::to_string(entity)).c_str(), &entityPosition[0], 0.05f, -FLT_MAX, +FLT_MAX, "%.2f", flags)) {
                transformComponent.setPosition(entityPosition);
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat(std::string("##y" + std::to_string(entity)).c_str(), &entityPosition[1], 0.05f, -FLT_MAX, +FLT_MAX, "%.2f", flags)) {
                transformComponent.setPosition(entityPosition);
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat(std::string("##z" + std::to_string(entity)).c_str(), &entityPosition[2], 0.05f, -FLT_MAX, +FLT_MAX, "%.2f", flags)) {
                transformComponent.setPosition(entityPosition);
            }
            ImGui::NextColumn();
            if (ImGui::Button(std::string("x##remove" + std::to_string(entity)).c_str())) {
                SceneManager::getInstance()->removeEntity(entity);
            }
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }
    if (ImGui::CollapsingHeader("Options", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static ImGuiSliderFlags flags = ImGuiSliderFlags_None;
        auto activeCamera = SceneManager::getInstance()->getActiveCamera();
        auto& cameraComponent = SceneManager::getInstance()->getComponent<CameraComponent>(*activeCamera, CAMERA_COMPONENT_ID);
        if (activeCamera)
        {
            static float cameraSpeed = cameraComponent.getInitialSpeed();
            ImGui::Text("Camera speed:");
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            if (ImGui::SliderFloat("##camera_speed_slider", &cameraSpeed, 1.0f, 100.0f, "%.3f", flags))
            {
                cameraComponent.setSpeed(cameraSpeed);
            }
        }
    }
    if (ImGui::CollapsingHeader("Splat Manipulation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Rendering method:");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        {
            renderingSystem->switchToPreviousSplatRenderer();
        }
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        {
            renderingSystem->switchToNextSplatRenderer();
        }
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        auto splatRenderer = renderingSystem->getSplatRenderer().get();
        auto splatRendererName = splatRenderer->getName();
        ImGui::Text("%s", splatRendererName.c_str());

        static float splatSize = 1.0f;
        static float gaussianSigma = 0.5f;
        static ImGuiSliderFlags flags = ImGuiSliderFlags_None;

        ImGui::Text("Splat size:");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::SliderFloat("##splat_size_slider", &splatSize, 0.0f, 1.0f, "%.3f", flags))
        {
            renderingSystem->updateSplatSize(splatSize);
        }

        if (splatRendererName == "gaussian_splat")
        {
            ImGui::Text("Gaussian value:");
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            if (ImGui::SliderFloat("##gaussian_value_slider", &gaussianSigma, 0.0f, 2.0f, "%.3f", flags))
            {
                static_cast<GaussianSplatRenderer*>(splatRenderer)->updateGaussianSigma(gaussianSigma);
            }
        }
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
            "It passes octree leaves positions (3d points) to the GPU and uses geometry shader to tranform them into splats, which are always facing the active camera."
            );
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiSystem::renderEntityColorPicker(EntityId entity)
{
    auto& graphicsComponent = SceneManager::getInstance()->getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);

    glm::vec4 c = graphicsComponent.getColor();
    ImVec4 color(c.r, c.g, c.b, c.a);

    static bool alpha_preview = true;
    static bool alpha_half_preview = false;
    static bool drag_and_drop = true;
    static bool options_menu = true;
    static bool hdr = false;
    ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

    if (ImGui::ColorEdit4(std::string("##entity_" + std::to_string(entity)).c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags))
    {
        graphicsComponent.setColor(glm::vec4(color.x, color.y, color.z, color.w));
    }
}
