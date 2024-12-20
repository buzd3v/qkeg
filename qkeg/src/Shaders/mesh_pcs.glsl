#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require

#include "scene.glsl"
#include "vertex.glsl"

layout (push_constant, scalar) uniform constants
{
    VertexBuffer vertexBuffer;
    SceneDataBuffer sceneData;
    mat4 transform;
    uint materialID;
    uint padding;
} pcs;