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
        glm::vec3 tangent;
    };

    std::vector<Vertex> vertices;

    std::string name{};
    glm::vec3   minPos{};
    glm::vec3   maxPos{};
};