#pragma once
#include "System.h"
#include "Shader.h"
#include "Camera.h"

class RenderingSystem : public System
{
public:
	RenderingSystem(GLFWwindow* window, int windowWidth, int windowHeight);
	virtual void update(Scene& scene, float deltaTime);

private:
	ShaderProgram shaderProgram_;
	Camera camera_;
};
