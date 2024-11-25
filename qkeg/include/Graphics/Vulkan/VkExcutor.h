#pragma once
#include <functional>
#include <singleton_atomic.hpp>
#include <vulkan/vulkan.h>

// for immediately excute command
class VkExcutor : public SingletonAtomic<VkExcutor>
{
  public:
    void init(VkDevice device, VkQueue graphicsQueue, uint32_t graphicsQueueFamily);
    void cleanup(VkDevice device);
    void submit(std::function<void(VkCommandBuffer cmd)> &&function);

  private:
    VkDevice        device;
    VkQueue         graphicsQueue;
    VkCommandBuffer immCmdBuffer;
    VkCommandPool   immCmdPool;
    VkFence         immFence;
};