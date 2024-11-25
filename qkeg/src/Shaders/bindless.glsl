#extension GL_EXT_nonuniform_qualifier : enable 

#define NEAREST_SAMPLER 0
#define LINEAR_SAMPLER 1
#define SHADOW_SAMPLER 2

layout(set = 0, binding = 0) uniform texture2D textures[];
layout(set = 0, binding = 1) uniform sampler samplers[]; // Changed to sampler type

vec4 samplerTexture2D(uint texID, vec2 uv, uint samplerType)
{
    switch(samplerType)
    {
        case NEAREST_SAMPLER:
            return texture(nonuniformEXT(sampler2D(textures[texID], samplers[NEAREST_SAMPLER])), uv);
        case LINEAR_SAMPLER:
            return texture(nonuniformEXT(sampler2D(textures[texID], samplers[LINEAR_SAMPLER])), uv);
        case SHADOW_SAMPLER:
            return texture(nonuniformEXT(sampler2D(textures[texID], samplers[SHADOW_SAMPLER])), uv);
        default:
            return vec4(0.0); // Return a default value if no match
    }
}
