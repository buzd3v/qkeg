#pragma once
// clang-format off
#include "GPUDevice.h"
// clang-format on

struct GPUBuffer
{
    VkBuffer          buffer;
    VmaAllocation     allocation;
    VmaAllocationInfo info;

    VkDeviceAddress address{0};

    void cleanup(GPUDevice &device) { vmaDestroyBuffer(device.getGlobalAllocator(), buffer, allocation); }
};

class GPUBufferBuilder
{
  public:
    GPUBufferBuilder()                                    = delete;
    GPUBufferBuilder(GPUBufferBuilder &other)             = delete;
    GPUBufferBuilder(GPUBufferBuilder &&other)            = delete;
    GPUBufferBuilder &operator=(GPUBufferBuilder &&other) = delete;
    GPUBufferBuilder &operator=(GPUBufferBuilder &other)  = delete;

    GPUBufferBuilder(GPUDevice &device, std::size_t allocSize);

    GPUBufferBuilder &setBufferUsageFlags(VkBufferUsageFlags usage);
    GPUBufferBuilder &setMemoryUsageFlags(VmaMemoryUsage memoryUsage);

    GPUBuffer build();

  private:
    GPUDevice         *device;
    std::size_t        allocSize;
    VkBufferUsageFlags usage;
    VmaMemoryUsage     memoryUsage;
};