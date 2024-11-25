#version 460
#extension GL_EXT_scalar_block_layout : enable 

#include "bindless.glsl"

layout(push_constant, scalar) uniform constants
{
    uint textureID;
} pcs;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

// //descriptor bindings for the pipeline
// layout(rgba16f,set = 0, binding = 0) uniform image2D image;


void main() 
{
     // Calculate the interpolation factor based on the screen's Y coordinate
    vec2 factor = gl_FragCoord.xy / vec2(800,600);

    // Interpolate between colorBottom and colorTop based on the factor
    // vec3 color = vec3(0.f, 0.f, factor);

    // Set the final fragment color
    outColor = vec4(factor,0.0, 1.0);
}