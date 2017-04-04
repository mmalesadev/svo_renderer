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
	GLFWwindow * window;
	SceneManager sceneManager;

	std::string windowTitle;
	int FPScount = 0;

	std::vector<System> systems;
	
	static void errorCallback(int error, const char * description);

public:
	void init();
	void run();

	ProgramContext();
	~ProgramContext();
};

