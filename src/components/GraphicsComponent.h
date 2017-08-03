#pragma once

#include <GL/glew.h>

class GraphicsComponent
{
public:
	virtual ~GraphicsComponent() = default;

	virtual void render() = 0;

protected:
	GLuint VAO_;
	GLuint VBO_;
};