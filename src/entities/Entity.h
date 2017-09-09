#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include "GraphicsComponent.h"
#include "ControllerComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "SVOComponent.h"

class Entity
{
public:
    Entity(unsigned int id, std::string name, std::unique_ptr<TransformComponent>& transformComponent, std::unique_ptr<GraphicsComponent>& graphicsComponent,
        std::unique_ptr<CameraComponent>& cameraComponent);

    ~Entity();

    Entity(const Entity&);

    std::string getName();
    std::unique_ptr<TransformComponent>& getTransformComponent();
    std::unique_ptr<GraphicsComponent>& getGraphicsComponent();
    std::unique_ptr<CameraComponent>& getCameraComponent();
    std::unique_ptr<ControllerComponent>& getControllerComponent();

private:
    unsigned int id_;
    std::string name_;
    std::unique_ptr<TransformComponent> transformComponent_;
    std::unique_ptr<GraphicsComponent> graphicsComponent_;
    std::unique_ptr<CameraComponent> cameraComponent_;
    std::unique_ptr<ControllerComponent> controllerComponent_;
};