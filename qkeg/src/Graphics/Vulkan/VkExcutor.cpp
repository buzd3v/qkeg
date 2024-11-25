#include <Vulkan/VkExcutor.h>
#include <Vulkan/VkInitializer.h>
#include <Vulkan/VkUtil.h>

void VkExcutor::init(VkDevice device, VkQueue graphicsQueue, uint32_t graphicsQueueFamily)
{

    this->device        = device;
    this->graphicsQueue = graphicsQueue;

    auto poolCreateInfo =
        VkInitializer::commandPoolCreateInfo(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VK_CHECK(vkCreateCommandPool(device, &poolCreateInfo, nullptr, &immCmdPool));

    auto cmdCreateInfo = VkInitializer::commandBufferAlocInfo(immCmdPool, 1);
    VK_CHECK(vkAllocateCommandBuffers(device, &cmdCreateInfo, &immCmdBuffer));

    auto fenceCreateInfo = VkInitializer::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &immFence));
}

void VkExcutor::cleanup(VkDevice device)
{
    vkDestroyCommandPool(device, immCmdPool, nullptr);
    vkDestroyFence(device, immFence, nullptr);
}

void VkExcutor::submit(std::function<void(VkCommandBuffer cmd)> &&function)
{

    VK_CHECK(vkResetFences(device, 1, &immFence));
    VK_CHECK(vkResetCommandBuffer(immCmdBuffer, 0));

    const auto cmd = immCmdBuffer;

    const auto cmdBeginInfo = VkInitializer::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
    function(cmd);
    VK_CHECK(vkEndCommandBuffer(cmd));

    auto cmdSubmitInfo = VkInitializer::commandBufferSubmitInfo(cmd);
    auto submitInfo    = VkInitializer::submitInfo(&cmdSubmitInfo, nullptr, nullptr);

    VK_CHECK(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, immFence));
    VK_CHECK(vkWaitForFences(device, 1, &immFence, true, VkUtil::TIMEOUT));
}
