#version 330

layout (location = 0) in vec3 voxelPosition;
layout (location = 1) in vec3 voxelColor;
layout (location = 2) in vec3 voxelNormal;

uniform mat4 MV;

struct DirectionalLight
{
    vec3 color;
    vec3 directionViewSpace;
    float ambientIntensity;
};

uniform DirectionalLight sunLight;

out voxelData
{
    vec4 color;
    vec3 normal;
} voxel;

vec3 calculateVoxelColor(vec3 voxelPosViewSpace, vec3 voxelNormalViewSpace)
{
    // Diffuse
    vec3 lightDirectionViewSpace = voxelPosViewSpace;   // vector from camera pos (0, 0, 0) to voxel pos == voxel pos
    vec3 n = normalize(voxelNormalViewSpace);
    vec3 l = normalize(-lightDirectionViewSpace);
    float cosTheta = clamp(dot(n, l), 0, 1);

    // Specular
    vec3 eyeDirectionViewSpace = vec3(0, 0, 0) - voxelPosViewSpace;
	vec3 E = normalize(eyeDirectionViewSpace);
	vec3 R = reflect(-l, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);

    vec3 ambientVoxelColor = voxelColor * sunLight.ambientIntensity;
    vec3 diffuseVoxelColor = voxelColor * sunLight.color * cosTheta;
    vec3 specularVoxelColor = vec3(0.1, 0.1, 0.1) * sunLight.color * pow(cosAlpha, 5);

    return ambientVoxelColor + diffuseVoxelColor + specularVoxelColor;
}

void main()
{
    gl_Position = vec4(voxelPosition, 1.0);
    vec3 voxelNormalViewSpace = (MV * vec4(voxelNormal, 0)).xyz;
    vec3 voxelPosViewSpace = (MV * gl_Position).xyz;
    voxel.color = vec4(calculateVoxelColor(voxelPosViewSpace, voxelNormalViewSpace), 1.0);
    voxel.normal = voxelNormal;
}