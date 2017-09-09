#version 330

layout (location = 0) in vec3 voxelPosition;
layout (location = 1) in vec3 voxelColor;

uniform mat4 MV;

out voxelData
{
	vec4 color;
} voxel;

void main()
{
	gl_Position = MV * vec4(voxelPosition, 1.0);
	voxel.color = vec4(voxelColor, 1.0);
}