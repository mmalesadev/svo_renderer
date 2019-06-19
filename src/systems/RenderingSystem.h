#pragma once
#include "System.h"
#include "Shader.h"
#include "ProgramVariables.h"

class RenderingSystem : public System
{
public:
    RenderingSystem();
    virtual void update();

private:
    ShaderProgram shaderProgram_;
};
