#pragma once
#include "System.h"
#include "Shader.h"

class RenderingSystem : public System
{
public:
    RenderingSystem();
    virtual void update(float deltaTime);

private:
    ShaderProgram shaderProgram_;
};
