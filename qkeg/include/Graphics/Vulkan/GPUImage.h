#pragma once

#include <glm/vec2.hpp>
#include <limits>
#include <string>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

using ImageId = uint32_t;

namespace qConstant
{
static const VkImageUsageFlags defaultDrawImageUsage{VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT};
static const VkImageUsageFlags defaultDepthImageUsage{VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                                      VK_IMAGE_USAGE_SAMPLED_BIT};
static const VkImageUsageFlags defaultResolveImageUsage{
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
    VK_IMAGE_USAGE_SAMPLED_BIT};

}; // namespace qConstant

namespace qTypes
{
static const ImageId NULL_IMAGE_ID = std::numeric_limits<std::uint32_t>::max();
};

static auto defaultImageAllocationInfo = VmaAllocationCreateInfo{
    .usage          = VMA_MEMORY_USAGE_AUTO,
    .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
};

struct GPUImageCreateInfo
{
    VkFormat              format;
    VkImageUsageFlags     usageFlags;
    VkImageCreateFlags    createFlags;
    VkExtent3D            extent{};
    std::uint32_t         numLayers{1};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
    VkImageTiling         tiling{VK_IMAGE_TILING_OPTIMAL};
    bool                  mipMap{false};
    bool                  isCubemap{false};
};

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

    glm::ivec2    getSize2D() const { return glm::ivec2{extent.width, extent.height}; }
    VkExtent2D    getExtent2D() const { return VkExtent2D{extent.width, extent.height}; }
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

    bool isInitialized() const { return id != NULL_BINDLESS_ID; }

  private:
    std::uint32_t id{NULL_BINDLESS_ID};
};