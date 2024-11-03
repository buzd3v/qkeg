#pragma once
// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
// clang-format on

namespace Graphics
{
static constexpr int FRAME_ON_FLIGHT = 2;
};

namespace VkInitializer
{
// cmd pool
VkCommandPoolCreateInfo     commandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
VkCommandBufferAllocateInfo commandBufferAlocInfo(VkCommandPool pool, uint32_t count = 1);
VkCommandBufferSubmitInfo   commandBufferSubmitInfo(VkCommandBuffer cmd);
VkCommandBufferBeginInfo    commandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0,
                                                   VkCommandBufferBeginInfo *pnext = nullptr);

// sync structure
VkFenceCreateInfo     fenceCreateInfo(VkFenceCreateFlags flags = 0);
VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);
VkSemaphoreSubmitInfo semaphoreSubmitInfo(VkPipelineStageFlags2 stagemask, VkSemaphore semaphore);
VkSubmitInfo2         submitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphore,
                                 VkSemaphoreSubmitInfo *waitSemaphore);
VkPresentInfoKHR      presentInfo(VkSwapchainKHR *pSwapchain, VkSemaphore *pWaitSemaphore, uint32_t *imageIndex,
                                  VkPresentInfoKHR *pNext = nullptr);

VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspectMask);

}; // namespace VkInitializer