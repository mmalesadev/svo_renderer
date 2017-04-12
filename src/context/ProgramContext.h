#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <System.h>
#include "SceneManager.h"

class ProgramContext
{
private:
	GLFWwindow * window_;
	SceneManager sceneManager_;

	std::string windowTitle_;
	int FPScount_ = 0;

	std::vector<System> systems_;
	
	static void errorCallback(int error, const char * description);

public:
	void init();
	void run();

	ProgramContext();
	~ProgramContext();
};

