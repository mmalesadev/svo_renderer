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
    ShaderProgram mainShaderProgram_;
    ShaderProgram boundingBoxShaderProgram_;

    std::pair<int, int> windowSize_;
};
