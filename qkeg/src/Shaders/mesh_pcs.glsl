#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require

#include "scene.glsl"
#include "vertex.glsl"

layout (push_constant, scalar) uniform constants
{
    mat4 transform;
    SceneBuffer sceneData;
    VertexBuffer vertexBuffer;
    uint materialID;
    uint padding;
} pcs;