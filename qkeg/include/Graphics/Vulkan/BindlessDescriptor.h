#pragma once

#include <cstdint>
// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
// clang-format on

class BindlessDescriptor
{
  public:
    BindlessDescriptor() = default;

    void init(VkDevice device, float maxAnisotropy);
    void cleanUp(VkDevice device);

    void addImage(VkDevice device, std::uint32_t id, VkImageView imageView);
    void addSampler(VkDevice device, std::uint32_t id, VkSampler sampler);

  private:
    VkSampler linearSampler;
    VkSampler nearestSampler;
    VkSampler shadowMapSampler;

    VkDescriptorPool      bindlessPool;
    VkDescriptorSet       bindlessSet;
    VkDescriptorSetLayout bindlessLayout;
};