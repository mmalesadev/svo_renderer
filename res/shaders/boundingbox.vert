#version 330

layout (location = 0) in vec4 position;

uniform mat4 MV;
uniform mat4 P;

void main()
{
    gl_Position = P * MV * position;
}