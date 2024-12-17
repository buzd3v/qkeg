#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <glm/glm.hpp>

struct MeshProps
{
    std::vector<uint32_t> indices;

    // struct alignment
    struct Vertex
    {
        glm::vec3 position;
        float     uv_x;
        glm::vec3 normal;
        float     uv_y;
        glm::vec4 tangent;
    };

    std::vector<Vertex> vertices;

    struct SkinningProps {
        glm::vec<4, std::uint32_t> jointIds;
        glm::vec4 weights;
    };
    std::vector<SkinningProps> skinningProps;
    bool hasSkeleton{false};

    std::string name{};
    glm::vec3   minPos{};
    glm::vec3   maxPos{};
};