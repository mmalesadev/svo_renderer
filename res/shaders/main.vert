#version 330

layout (location = 0) in vec3 voxelPosition;
layout (location = 1) in vec3 voxelColor;
layout (location = 2) in vec3 voxelNormal;

uniform mat4 MV;

out voxelData
{
    vec4 color;
    vec3 normal;
} voxel;

void main()
{
    gl_Position = MV * vec4(voxelPosition, 1.0);
    voxel.color = vec4(voxelColor, 1.0);
    voxel.normal = voxel.normal;
}