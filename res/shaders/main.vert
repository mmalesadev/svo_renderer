#version 330

layout (location = 0) in vec3 voxelPosition;

uniform mat4 MV;

void main()
{
	gl_Position = MV * vec4(voxelPosition, 1.0);
}