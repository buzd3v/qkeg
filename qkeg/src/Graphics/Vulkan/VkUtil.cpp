#include "Graphics/Vulkan/VkUtil.h"
#include "Graphics/Vulkan/VkInitializer.h"

#include <fstream>
namespace VkUtil
{

void transitionImage(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
    VkImageAspectFlags aspectMask =
        (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    auto imageBarrier = VkImageMemoryBarrier2{
        .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext            = nullptr,
        .srcStageMask     = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        .srcAccessMask    = VK_ACCESS_2_MEMORY_WRITE_BIT,
        .dstStageMask     = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .dstAccessMask    = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
        .oldLayout        = currentLayout,
        .newLayout        = newLayout,
        .image            = image,
        .subresourceRange = VkInitializer::imageSubresourceRange(aspectMask),
    };

    auto depInfo = VkDependencyInfo{
        .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                   = nullptr,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &imageBarrier,
    };

    vkCmdPipelineBarrier2(cmdBuf, &depInfo);
}
void copyImageToImage(VkCommandBuffer cmdBuf, VkImage src, VkImage dst, VkExtent2D srcSize, VkExtent2D dstSize) {}
void generateMipmaps(VkCommandBuffer cmdBuf, VkImage image, VkExtent2D imgSize) {}
VkShaderModule loadShaderModule(const char *path, VkDevice device)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        fmt::println("Cannot load {}", path);
        return VK_NULL_HANDLE;
    }

    size_t                fileSize = (size_t)file.tellg();
    std::vector<uint32_t> buffer(fileSize / sizeof(std::uint32_t));
    file.seekg(0);
    file.read((char *)buffer.data(), fileSize);
    file.close();

    auto createInfo = VkShaderModuleCreateInfo{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .codeSize = buffer.size() * sizeof(uint32_t),
        .pCode    = buffer.data(),
    };
    VkShaderModule createModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &createModule) != VK_SUCCESS)
    {
        fmt::println("cannot create shadermodule for file {}", std::string(path));
    }
    return createModule;
}

VkPipelineLayout createPipelineLayout(VkDevice device, std::span<VkDescriptorSetLayout> layouts,
                                      std::span<VkPushConstantRange> pushConstantRange)
{
    const auto &createInfo = VkPipelineLayoutCreateInfo{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount         = (std::uint32_t)layouts.size(),
        .pSetLayouts            = layouts.data(),
        .pushConstantRangeCount = (std::uint32_t)pushConstantRange.size(),
        .pPushConstantRanges    = pushConstantRange.data(),
    };

    VkPipelineLayout layout;
    VK_CHECK(vkCreatePipelineLayout(device, &createInfo, nullptr, &layout));

    return layout;
}
} // namespace VkUtil