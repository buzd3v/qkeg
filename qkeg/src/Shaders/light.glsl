#ifndef LIGHT_GLSL
#define LIGHT_GLSL

#extension GL_EXT_buffer_reference : require

#define TYPE_LIGHT_DIRECTIONAL  0
#define TYPE_LIGHT_POINT        1
#define TYPE_LIGHT_SPOT         2

struct Light
{
    vec3  position;
    uint  type;
    vec3  direction;
    float range;
    vec3  color;
    float intensity;
    vec2  scaleOffset;
    uint  shadowMapId;
    float unused;
};
layout (buffer_reference, scalar) readonly buffer LightBuffer {
    Light data[];
};

#endif