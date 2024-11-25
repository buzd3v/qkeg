#include "Vulkan/GPUBuffer.h"

GPUBufferBuilder::GPUBufferBuilder(GPUDevice &device, std::size_t allocSize)
{
    this->device      = &device;
    this->memoryUsage = VMA_MEMORY_USAGE_AUTO;
    this->usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    this->allocSize   = allocSize;
}

GPUBufferBuilder &GPUBufferBuilder::setBufferUsageFlags(VkBufferUsageFlags usage)
{
    this->usage = usage;
    return *this;
}
GPUBufferBuilder &GPUBufferBuilder::setMemoryUsageFlags(VmaMemoryUsage memoryUsage)
{
    this->memoryUsage = memoryUsage;
    return *this;
}

GPUBuffer GPUBufferBuilder::build()
{

    VkDevice vkDev = device->getDevice();

    const auto bufferInfo = VkBufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size  = allocSize,
        .usage = usage,
    };

    const auto allocInfo = VmaAllocationCreateInfo{
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .usage = memoryUsage,
    };

    GPUBuffer buffer{};
    VK_CHECK(vmaCreateBuffer(
        device->getGlobalAllocator(), &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, &buffer.info));

    // REQUESTING BUFFER ADRESS INFO IN NEED
    if ((usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0)
    {
        const auto deviceAdressInfo = VkBufferDeviceAddressInfo{
            .sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = buffer.buffer,
        };
        buffer.address = vkGetBufferDeviceAddress(device->getDevice(), &deviceAdressInfo);
    }

    return buffer;
}