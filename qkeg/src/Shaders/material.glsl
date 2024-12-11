#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL

#extension GL_EXT_buffer_reference : require

struct Material
{
    vec4 baseColor;
    vec4 metalRoughnessEmissive;
    uint diffuseTextureId;
    uint normalTextureId;
    uint metallicRoughnessId;
    uint emissiveTextureId;
};

layout (buffer_reference, scalar) readonly buffer MaterialBuffer {
    Material data[];
};


#endif