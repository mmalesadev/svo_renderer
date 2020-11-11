#version 330

in vec4 color;
in vec3 normal;
in vec2 texCoords;

out vec4 outputColor;

uniform sampler2D splatTexture;
uniform float midEdgeAlphaValue;
uniform vec4 splatColorMultiplier;

void main()
{
    vec4 texel = texture2D(splatTexture, texCoords);
    if (texel.r < midEdgeAlphaValue)
        discard;
    outputColor = splatColorMultiplier * color;
}