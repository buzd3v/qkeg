#include <Graphics/Vulkan/VkInitializer.h>

namespace VkInitializer
{

VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
    auto createInfo = VkCommandPoolCreateInfo{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = flags,
        .queueFamilyIndex = queueFamilyIndex,
    };
    return createInfo;
}
VkCommandBufferAllocateInfo commandBufferAlocInfo(VkCommandPool pool, uint32_t count)
{
    auto createInfo = VkCommandBufferAllocateInfo{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count,
    };

    return createInfo;
}

VkCommandBufferSubmitInfo commandBufferSubmitInfo(VkCommandBuffer cmd)
{
    auto info = VkCommandBufferSubmitInfo{
        .sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext         = nullptr,
        .commandBuffer = cmd,
        .deviceMask    = 0,
    };
    return info;
}

VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags, VkCommandBufferBeginInfo *pnext)
{
    auto inf = VkCommandBufferBeginInfo{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = pnext,
        .flags            = flags,
        .pInheritanceInfo = nullptr,
    };

    return inf;
}

VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags)
{
    auto createInfo = VkFenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
    };

    return createInfo;
}
VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags)
{
    auto createInfo = VkSemaphoreCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
    };
    return createInfo;
}

VkSubmitInfo2 submitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphore,
                         VkSemaphoreSubmitInfo *waitSemaphore)
{
    auto submitInf = VkSubmitInfo2{
        .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext                    = nullptr,
        .waitSemaphoreInfoCount   = static_cast<uint32_t>((waitSemaphore == nullptr) ? 0 : 1),
        .pWaitSemaphoreInfos      = waitSemaphore,
        .commandBufferInfoCount   = 1,
        .pCommandBufferInfos      = cmd,
        .signalSemaphoreInfoCount = static_cast<uint32_t>((signalSemaphore == nullptr) ? 0 : 1),
        .pSignalSemaphoreInfos    = signalSemaphore,
    };
    return submitInf;
}

VkSemaphoreSubmitInfo semaphoreSubmitInfo(VkPipelineStageFlags2 stagemask, VkSemaphore semaphore)
{
    auto info = VkSemaphoreSubmitInfo{
        .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext       = nullptr,
        .semaphore   = semaphore,
        .value       = 1,
        .stageMask   = stagemask,
        .deviceIndex = 0,
    };

    return info;
}

VkPresentInfoKHR presentInfo(VkSwapchainKHR *pSwapchain, VkSemaphore *pWaitSemaphore, uint32_t *imageIndex,
                             VkPresentInfoKHR *pNext)
{
    auto info = VkPresentInfoKHR{
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = pNext,
        .waitSemaphoreCount = static_cast<uint32_t>(pWaitSemaphore == nullptr ? 0 : 1),
        .pWaitSemaphores    = pWaitSemaphore,
        .swapchainCount     = static_cast<uint32_t>(pSwapchain == nullptr ? 0 : 1),
        .pSwapchains        = pSwapchain,
        .pImageIndices      = imageIndex,
    };

    return info;
}

VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspectMask)
{
    auto subImage = VkImageSubresourceRange{
        .aspectMask     = aspectMask,
        .baseMipLevel   = 0,
        .levelCount     = VK_REMAINING_MIP_LEVELS,
        .baseArrayLayer = 0,
        .layerCount     = VK_REMAINING_ARRAY_LAYERS,
    };

    return subImage;
}

VkPipelineShaderStageCreateInfo VkInitializer::shaderStageCreateInfo(VkShaderStageFlagBits flags, VkShaderModule module)
{
    return VkPipelineShaderStageCreateInfo{
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = flags,
        .module = module,
        .pName  = "main",
    };
}
} // namespace VkInitializer
