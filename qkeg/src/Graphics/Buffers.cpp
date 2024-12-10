#include "Buffers.h"

void Buffers::cleanup(GPUDevice &device)
{
    for (auto &stagingBuffer : stagingBuffers)
    {
        stagingBuffer.cleanup(device);
    }
    stagingBuffers.clear();

    headBuffer.cleanup(device);

    isInit = false;
}

void Buffers::uploadData(VkCommandBuffer cmd, size_t bufferIndex, void *data, size_t dataSize, size_t offset, bool sync)
{
    assert(bufferIndex < nBuffers);
    assert(isInit);
    assert(dataSize + offset <= bufferSize);

    if (dataSize == 0)
        return; // do nothing;

    // syncing by adding a barrier to cmd
    if (sync)
    {
        const auto bufferBarrier = VkBufferMemoryBarrier2{
            .sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .srcAccessMask = VK_ACCESS_2_MEMORY_READ_BIT,
            .dstStageMask  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .buffer        = headBuffer.buffer,
            .offset        = 0,
            .size          = VK_WHOLE_SIZE,
        };
        const auto dependencyInfo = VkDependencyInfo{
            .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .bufferMemoryBarrierCount = 1,
            .pBufferMemoryBarriers    = &bufferBarrier,
        };
        vkCmdPipelineBarrier2(cmd, &dependencyInfo);
    }
    auto  stagingBuffer = stagingBuffers[bufferIndex];
    auto *mappedData    = reinterpret_cast<uint8_t *>(stagingBuffer.info.pMappedData);
    memcpy((void *)mappedData[offset], data, dataSize);

    const VkBufferCopy2 region{
        .sType     = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .srcOffset = (VkDeviceSize)offset,
        .dstOffset = (VkDeviceSize)offset,
        .size      = dataSize,
    };

    const VkCopyBufferInfo2 bufferCopyInfo{
        .sType       = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .srcBuffer   = stagingBuffer.buffer,
        .dstBuffer   = headBuffer.buffer,
        .regionCount = 1,
        .pRegions    = &region,
    };

    vkCmdCopyBuffer2(cmd, &bufferCopyInfo);

    if (sync)
    {
        const VkBufferMemoryBarrier2 bufferBarrier{
            .sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .srcStageMask  = VK_PIPELINE_STAGE_2_COPY_BIT,
            .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
            .buffer        = headBuffer.buffer,
            .offset        = 0,
            .size          = VK_WHOLE_SIZE,
        };
        const VkDependencyInfo dependencyInfo{
            .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .bufferMemoryBarrierCount = 1,
            .pBufferMemoryBarriers    = &bufferBarrier,
        };
        vkCmdPipelineBarrier2(cmd, &dependencyInfo);
    }
}
void Buffers::init(GPUDevice &device, VkBufferUsageFlags flags, size_t nBuffers, size_t bufferSize)
{
    this->nBuffers   = nBuffers;
    this->bufferSize = bufferSize;

    headBuffer = GPUBufferBuilder{device, bufferSize}
                     .setBufferUsageFlags(flags | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                     .setMemoryUsageFlags(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
                     .build();
    for (auto i = 0; i < nBuffers; i++)
    {
        auto buffer = GPUBufferBuilder{device, bufferSize}
                          .setBufferUsageFlags(flags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
                          .setMemoryUsageFlags(VMA_MEMORY_USAGE_AUTO_PREFER_HOST)
                          .build();
        stagingBuffers.push_back(buffer);
    }
    isInit = true;
}
