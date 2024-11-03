#pragma once

#include <glm/vec2.hpp>
#include <limits>
#include <string>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

struct GPUImage
{
    VkImage           image;
    VkImageView       imageView;
    VkExtent3D        extent;
    VkFormat          format;
    VkImageUsageFlags usage;
    std::uint32_t     mipLevels{1};
    std::uint32_t     numLayers{1};
    std::string       debugName{""};
    bool              isCubeMap{false};

    // alloc image
    VmaAllocation     allocation;
    static const auto NULL_BINDLESS_ID = std::numeric_limits<std::uint32_t>::max();

    glm::ivec2    getSize2D() { return glm::ivec2{extent.width, extent.height}; }
    VkExtent2D    getExtent2D() { return VkExtent2D{extent.width, extent.height}; }
    std::uint32_t getImageId() const
    {
        assert(this->id != NULL_BINDLESS_ID && "ImageID is not exist!");
        return id;
    }
    void setImageId(std::uint32_t id)
    {
        assert(id != NULL_BINDLESS_ID);
        this->id = id;
    }

  private:
    std::uint32_t id{NULL_BINDLESS_ID};
};