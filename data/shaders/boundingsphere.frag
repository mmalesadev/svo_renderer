#version 330

out vec4 outputColor;

struct DirectionalLight
{
    vec3 color;
    vec3 directionCameraspace;
    float ambientIntensity;
};

void main()
{
    outputColor = vec4(1.0, 0.0, 0.0, 1.0);
}