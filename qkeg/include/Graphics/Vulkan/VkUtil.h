#pragma once

// clang-format off
#include <assert.h>
#include <fmt/format.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h>
// clang-format on

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

namespace VkUtil
{
void transitionImage(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
void copyImageToImage(VkCommandBuffer cmdBuf, VkImage src, VkImage dst, VkExtent2D srcSize, VkExtent2D dstSize);
void generateMipmaps(VkCommandBuffer cmdBuf, VkImage image, VkExtent2D imgSize);
VkShaderModule loadShaderModule(const char *file, VkDevice device);

VkPipelineLayout createPipelineLayout(VkDevice device, std::span<VkDescriptorSetLayout> layouts = {},
                                      std::span<VkPushConstantRange> pushConstantRange = {});
} // namespace VkUtil