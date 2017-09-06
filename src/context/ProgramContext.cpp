#include "ProgramContext.h"
#include <GLFW/glfw3.h>
#include "RenderingSystem.h"
#include <iostream>
#include <memory>

ProgramContext::ProgramContext()
{
	glfwSetErrorCallback( (GLFWerrorfun)errorCallback );
}

ProgramContext::~ProgramContext()
{
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

	GLFWmonitor * monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode * desktop = glfwGetVideoMode(monitor);
	window_ = glfwCreateWindow(desktop->width, desktop->height, windowTitle_.c_str(), nullptr, nullptr);

	glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwMakeContextCurrent(window_);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwSetCursorPos(window_, desktop->width / 2, desktop->height / 2);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
	{
		std::cout << "Error initializing GLEW: " << glewGetErrorString(glewErr) << std::endl;
	}

	sceneManager_.activateScene("default");

	int windowWidth, windowHeight;
	glfwGetWindowSize(window_, &windowWidth, &windowHeight);

	systems_.push_back(std::make_unique<RenderingSystem>(window_, windowWidth, windowHeight));
}

void ProgramContext::run()
{
	do
	{
		static GLdouble lastTime = glfwGetTime();
		static GLdouble fpsLastTime = 0;
		GLdouble currentTime = glfwGetTime();
		GLfloat deltaTime = GLfloat(currentTime - lastTime);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update the systems
		for (auto& system : systems_) system->update(sceneManager_.getActiveScene(), deltaTime);

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
	} while (!glfwWindowShouldClose(window_));
}
