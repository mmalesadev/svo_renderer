#include "ProgramContext.h"
#include <GLFW/glfw3.h>
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
	window = glfwCreateWindow(desktop->width, desktop->height, windowTitle.c_str(), nullptr, nullptr);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwMakeContextCurrent(window);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwSetCursorPos(window, desktop->width / 2, desktop->height / 2);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
	{
		std::cout << "Error initializing GLEW: " << glewGetErrorString(glewErr) << std::endl;
	}

	sceneManager.loadScene("default");
	sceneManager.activateScene("default");
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
		for (System& system : systems) system.update(sceneManager.getActiveScene()->getWorld());
		//screen->update(deltaTime);

		//screen->render();

		++FPScount;
		if (currentTime - fpsLastTime >= 1)
		{
			//screen->FPScount = FPScount;
			std::cout << "FPS: " << FPScount << std::endl;
			FPScount = 0;
			fpsLastTime = currentTime;
		}
		lastTime = currentTime;

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));
}
