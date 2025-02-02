#version 460

#extension GL_GOOGLE_include_directive : require

#include "bindless.glsl"

layout (location = 0) in vec2 inUV;

layout (push_constant) uniform constants
{
	mat4 invViewProj;
    vec4 cameraPos;
    uint skyboxTexId;
} pcs;

layout (location = 0) out vec4 outFragColor;

void main() {
    vec2 uv = inUV;
    vec2 ndc = uv * 2.0 - vec2(1.0);

    vec4 coord = pcs.invViewProj * vec4(ndc, 1.0, 1.0);
    vec3 samplePoint = normalize(coord.xyz / vec3(coord.w) - pcs.cameraPos.xyz);

    outFragColor = vec4(sampleTextureCubeLinear(pcs.skyboxTexId, samplePoint));
}
