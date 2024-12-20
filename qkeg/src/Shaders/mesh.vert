#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "scene.glsl"
#include "mesh_pcs.glsl"

layout (location = 0) out vec3 outPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec4 outTangent;
layout (location = 4) out mat3 outTBN;

void main()
{
    Vertex vert = pcs.vertexBuffer.vertices[gl_VertexIndex];
    vec4 worldPos = pcs.transform * vec4(vert.position, 1.f);
    gl_Position = pcs.sceneData.viewProj * worldPos;

    outPos = worldPos.xyz;
    outUV  = vec2(vert.uv_x, vert.uv_y);
    outNormal = mat3(transpose(inverse(pcs.transform))) * vert.normal;
    outTangent = vert.tangent;

    vec3 T = normalize(vec3(pcs.transform * vert.tangent));
    vec3 N = normalize(outNormal);
    vec3 B = cross(N, T) * vert.tangent.w;
    outTBN = mat3(T, B, N);

}