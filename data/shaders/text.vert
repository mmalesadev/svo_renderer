#version 330

layout(location = 0) in vec2 vertexPositionInScreenSpace;
layout(location = 1) in vec2 vertexUV;

uniform int windowWidth;
uniform int windowHeight;

out vec2 UV;

void main()
{
	vec2 vertexPositionInHomogenousSpace = vertexPositionInScreenSpace - vec2(windowWidth/2, windowHeight/2);
	vertexPositionInHomogenousSpace /= vec2(windowWidth/2, windowHeight/2);
	gl_Position = vec4(vertexPositionInHomogenousSpace, 0, 1);

	UV = vertexUV;
}