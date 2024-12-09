#include "Graphics/Vulkan/VkUtil.h"
#include "Graphics/Vulkan/VkInitializer.h"

#include <fstream>
namespace VkUtil
{

void transitionImage(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
    VkImageAspectFlags aspectMask = (currentLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
                                     newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
                                     newLayout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)
                                        ? VK_IMAGE_ASPECT_DEPTH_BIT
                                        : VK_IMAGE_ASPECT_COLOR_BIT;

    auto imageBarrier = VkImageMemoryBarrier2{
        .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext            = nullptr,
        .srcStageMask     = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
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
void copyImageToImage(VkCommandBuffer cmdBuf, VkImage src, VkImage dst, VkExtent2D srcSize, VkExtent2D dstSize,
                      VkFilter filter)
{
    const auto blitRegion = VkImageBlit2{
        .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
        .pNext = nullptr,
        .srcSubresource =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
        .srcOffsets =
            {
                {},
                {.x = (int32_t)srcSize.width, .y = (int32_t)srcSize.height, .z = 1},
            },
        .dstSubresource =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
        .dstOffsets =
            {
                {0, 0},
                {.x = (int32_t)dstSize.width, .y = (int32_t)dstSize.height, .z = 1},
            },
    };

    const auto blitInfo = VkBlitImageInfo2{
        .sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .pNext          = nullptr,
        .srcImage       = src,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage       = dst,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount    = 1,
        .pRegions       = &blitRegion,
        .filter         = filter,
    };

    vkCmdBlitImage2(cmdBuf, &blitInfo);
}

void generateMipmaps(VkCommandBuffer cmdBuf, VkImage image, VkExtent2D imgSize, uint32_t mipLevels)
{
    for (std::uint32_t mip = 0; mip < mipLevels; mip++)
    {
        const auto halfExtent = VkExtent2D{
            .width  = imgSize.width / 2,
            .height = imgSize.height / 2,
        };

        const auto imgBarrier = VkImageMemoryBarrier2{
            .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext         = nullptr,
            .srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
            .oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .image         = image,
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = mip,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = VK_REMAINING_ARRAY_LAYERS,
                },
        };

        auto dependencyInfo = VkDependencyInfo{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
        };

        vkCmdPipelineBarrier2(cmdBuf, &dependencyInfo);

        if (mip < mipLevels - 1)
        {
            VkImageBlit2 blitRegion{
                .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
                .pNext = nullptr,
                .srcSubresource =
                    {
                        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel       = mip,
                        .baseArrayLayer = 0,
                        .layerCount     = 1,
                    },
                .srcOffsets = {{
                    .x = (int32_t)imgSize.width,
                    .y = (int32_t)imgSize.height,
                    .z = 1,
                }},
                .dstSubresource =
                    {
                        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel       = mip,
                        .baseArrayLayer = 0,
                        .layerCount     = 1,
                    },
                .dstOffsets = {{
                    .x = (int32_t)halfExtent.width,
                    .y = (int32_t)halfExtent.height,
                    .z = 1,
                }},
            };

            VkBlitImageInfo2 blitInfp{
                .sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
                .pNext          = nullptr,
                .srcImage       = image,
                .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                .dstImage       = image,
                .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .regionCount    = 1,
                .pRegions       = &blitRegion,
                .filter         = VK_FILTER_LINEAR,
            };

            vkCmdBlitImage2(cmdBuf, &blitInfp);
            imgSize = halfExtent;
        }
    }
    transitionImage(cmdBuf, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
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

RenderInfo createRenderingInfo(const RenderingCreateInfo &info)
{
    RenderInfo ri{};
    assert((info.colorImageView || info.depthImageView != nullptr) && "Either draw or depth image should be present");

    if (info.colorImageView)
    {
        ri.colorAttachment = VkRenderingAttachmentInfo{
            .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView   = info.colorImageView,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp  = info.colorImageClearValue.has_value() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };
        if (info.colorImageClearValue.has_value())
        {
            auto color                          = info.colorImageClearValue.value();
            ri.colorAttachment.clearValue.color = {(float)color.x, (float)color.y, (float)color.z, (float)color.w};
        }
    }
    if (info.depthImageView)
    {
        ri.depthAttachment = VkRenderingAttachmentInfo{
            .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView   = info.depthImageView,
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .loadOp  = info.depthImageClearValue.has_value() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };
        if (info.depthImageClearValue.has_value())
        {
            auto color                                       = info.depthImageClearValue.value();
            ri.depthAttachment.clearValue.depthStencil.depth = color;
        }
    }
    if (info.resolveImageView)
    {
        ri.colorAttachment.resolveImageView   = info.resolveImageView;
        ri.colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ri.colorAttachment.resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT;
    }

    ri.renderingInfo = VkRenderingInfo{
        .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea           = VkRect2D{.offset = {}, .extent = info.renderExtent},
        .layerCount           = 1,
        .colorAttachmentCount = info.colorImageView ? 1u : 0u,
        .pColorAttachments    = info.colorImageView ? &ri.colorAttachment : nullptr,
        .pDepthAttachment     = info.depthImageView ? &ri.depthAttachment : nullptr,
    };

    return ri;
    // return renderingInfo;
}
} // namespace VkUtil