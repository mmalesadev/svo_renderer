#version 330

in vec4 color;
in vec3 normal;

out vec4 outputColor;

struct DirectionalLight
{
    vec3 color;
    vec3 directionCameraspace;
    float ambientIntensity;
};

void main()
{
    outputColor = color;
}