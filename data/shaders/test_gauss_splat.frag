#version 330

in vec4 color;
in vec3 normal;

out vec4 outputColor;

uniform sampler2D splatTexture;

void main()
{
    outputColor = color;
}