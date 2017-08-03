#version 330

uniform mat4 MVP;

layout (location = 0) in vec3 voxelPosition;

void main()
{
	//gl_Position = vec4(vec3(voxelPosition), 1.0);
	gl_Position = MVP * vec4(vec3(voxelPosition), 1.0);
	//gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}