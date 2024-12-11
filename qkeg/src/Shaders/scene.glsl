#ifndef SCENE_GLSL
#define SCENE_GLSL

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "light.glsl"
#include "material.glsl"

struct SceneData {
    //camera data
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec4 cameraPos;

    vec3  ambientColor;
    float ambientIntensity;
    MaterialBuffer materials;
    // LightBuffer lights;
};

layout (buffer_reference, scalar) readonly buffer SceneBuffer
{
    SceneData scene;
}sceneDataBuffer;

#endif