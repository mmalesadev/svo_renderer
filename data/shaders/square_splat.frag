#version 330

in vec4 color;
in vec3 normal;

out vec4 outputColor;
uniform vec4 splatColorMultiplier;

void main()
{
    outputColor = splatColorMultiplier * color;
}