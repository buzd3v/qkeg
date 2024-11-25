#pragma once

// clang-format off
#include <assert.h>
#include <fmt/format.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h>
// clang-format on

#include <glm/vec4.hpp>
#include <optional>

#define VK_CHECK(x)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        VkResult err = x;                                                                                              \
        if (err)                                                                                                       \
        {                                                                                                              \
            fmt::print("Detected Vulkan error: {}", string_VkResult(err));                                             \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

struct RenderInfo
{
    VkRenderingAttachmentInfo colorAttachment;
    VkRenderingAttachmentInfo depthAttachment;
    VkRenderingInfo           renderingInfo;
};

struct RenderingCreateInfo
{
    VkExtent2D                renderExtent{};
    VkImageView               colorImageView{VK_NULL_HANDLE};
    VkImageView               depthImageView{VK_NULL_HANDLE};
    VkImageView               resolveImageView{VK_NULL_HANDLE};
    std::optional<glm::ivec4> colorImageClearValue;
    std::optional<glm::ivec4> depthImageClearValue;
};

namespace VkUtil
{
static constexpr auto TIMEOUT = std::numeric_limits<std::uint64_t>::max();

void transitionImage(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
void copyImageToImage(VkCommandBuffer cmdBuf, VkImage src, VkImage dst, VkExtent2D srcSize, VkExtent2D dstSize,
                      VkFilter filter);
void generateMipmaps(VkCommandBuffer cmdBuf, VkImage image, VkExtent2D imgSize, uint32_t mipLevels);
VkShaderModule loadShaderModule(const char *file, VkDevice device);

VkPipelineLayout createPipelineLayout(VkDevice device, std::span<VkDescriptorSetLayout> layouts = {},
                                      std::span<VkPushConstantRange> pushConstantRange = {});

RenderInfo createRenderingInfo(const RenderingCreateInfo &info);
} // namespace VkUtil
