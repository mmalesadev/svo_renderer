#version 330

uniform mat4 P;
uniform float scale;
uniform float gridLength;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main()
{
    vec4 voxelPosViewSpace = gl_in[0].gl_Position;

    float voxelOffset = (scale/gridLength) * 1.414;     // 1.414 = sqrt(2)
    
    vec2 bottomLeft = voxelPosViewSpace.xy + vec2(-0.5, -0.5) * voxelOffset;
    gl_Position = P * vec4(bottomLeft, voxelPosViewSpace.zw);
    EmitVertex();   

    vec2 bottomRight = voxelPosViewSpace.xy + vec2(0.5, -0.5) * voxelOffset;
    gl_Position = P * vec4(bottomRight, voxelPosViewSpace.zw);
    EmitVertex();

    vec2 topLeft = voxelPosViewSpace.xy + vec2(-0.5, 0.5) * voxelOffset;
    gl_Position = P * vec4(topLeft, voxelPosViewSpace.zw);
    EmitVertex();

    vec2 topRight = voxelPosViewSpace.xy + vec2(0.5, 0.5) * voxelOffset;
    gl_Position = P * vec4(topRight, voxelPosViewSpace.zw);
    EmitVertex();

	EndPrimitive();
}