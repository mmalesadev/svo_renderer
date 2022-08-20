#version 330

uniform mat4 MV;
uniform mat4 P;
uniform float scale;
uniform float gridLength;
uniform float splatSize;
uniform bool cullInvisibleFaces;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in voxelData
{
    vec4 color;
    vec3 normal;
} voxel[];

out vec4 color;
out vec3 normal;
out vec2 texCoords;

void main()
{
    vec4 voxelPosViewSpace = MV * gl_in[0].gl_Position;
    vec4 voxelPosClipSpace = P * voxelPosViewSpace;
    voxelPosClipSpace /= voxelPosClipSpace.w;

    float voxelOffset = splatSize * (scale/gridLength) * 1.414213;     // 1.414213 = sqrt(2)

    vec4 voxelRightEdgeMidPointClipSpace = P * vec4(voxelPosViewSpace.xy + vec2(0.5, 0) * voxelOffset, voxelPosViewSpace.zw);
    voxelRightEdgeMidPointClipSpace /= voxelRightEdgeMidPointClipSpace.w;

    float voxelHalfEdgeLengthClipSpace = distance(voxelPosClipSpace, voxelRightEdgeMidPointClipSpace);
    float voxelFrustumCullingBorder = 1 + voxelHalfEdgeLengthClipSpace;
   
    // Clip space frustum culling
    if (voxelPosClipSpace.x < -voxelFrustumCullingBorder || voxelPosClipSpace.x > voxelFrustumCullingBorder ||
        voxelPosClipSpace.y < -voxelFrustumCullingBorder || voxelPosClipSpace.y > voxelFrustumCullingBorder)
        return;

    color = voxel[0].color;
    normal = voxel[0].normal;

    vec4 normalViewSpace = MV * vec4(normal, 0);

    // Skip the voxel if its face is not seen by the camera
    if (cullInvisibleFaces && (dot(vec3(0,0,0) - voxelPosViewSpace.xyz, normalViewSpace.xyz)) < 0)
    {
        return;
    }

    vec2 bottomLeft = voxelPosViewSpace.xy + vec2(-0.5, -0.5) * voxelOffset;
    gl_Position = P * vec4(bottomLeft, voxelPosViewSpace.zw);
    texCoords = vec2(0, 0);
    EmitVertex();   

    vec2 bottomRight = voxelPosViewSpace.xy + vec2(0.5, -0.5) * voxelOffset;
    gl_Position = P * vec4(bottomRight, voxelPosViewSpace.zw);
    texCoords = vec2(1, 0);
    EmitVertex();

    vec2 topLeft = voxelPosViewSpace.xy + vec2(-0.5, 0.5) * voxelOffset;
    gl_Position = P * vec4(topLeft, voxelPosViewSpace.zw);
    texCoords = vec2(0, 1);
    EmitVertex();

    vec2 topRight = voxelPosViewSpace.xy + vec2(0.5, 0.5) * voxelOffset;
    gl_Position = P * vec4(topRight, voxelPosViewSpace.zw);
    texCoords = vec2(1, 1);
    EmitVertex();

    EndPrimitive();

}