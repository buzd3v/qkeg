#pragma once

#include <cstdint>
#include <singleton_atomic.hpp>
// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
// clang-format on

class BindlessDescriptor : public SingletonAtomic<BindlessDescriptor>
{
  public:
    BindlessDescriptor() = default;

    void init(VkDevice device, float maxAnisotropy);
    void cleanUp(VkDevice device);

    void addImage(VkDevice device, std::uint32_t id, VkImageView imageView);
    void addSampler(VkDevice device, std::uint32_t id, VkSampler sampler);

    void                   bindBindlessSet(VkCommandBuffer cmd, VkPipelineLayout layout, VkPipelineBindPoint bindPoint);
    VkDescriptorSetLayout &getDesSetLayout() { return bindlessLayout; }

  private:
    VkSampler linearSampler;
    VkSampler nearestSampler;
    VkSampler shadowMapSampler;

    VkDescriptorPool      bindlessPool;
    VkDescriptorSet       bindlessSet;
    VkDescriptorSetLayout bindlessLayout;
};