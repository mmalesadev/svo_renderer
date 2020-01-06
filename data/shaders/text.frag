#version 330

in vec2 UV;

out vec4 outputColor;

uniform sampler2D textureSampler;

void main()
{
	vec4 textureColor = texture(textureSampler, UV).rgba;

	outputColor = vec4(textureColor)*vec4(1.0f, 1.0f, 1.0f, 1.0f);
}