#pragma once

#include "Mesh/MeshPool.h"
class GPUDevice;

class MeshPipeline
{
  public:
    void init(GPUDevice &device, VkFormat colorImageFormat, VkFormat depthImageFormat,
              VkSampleCountFlagBits sampleCounts);
    void draw(VkCommandBuffer cmd, GPUDevice &device, VkExtent2D renderView, std::span<MeshId> meshList);

  private:
    struct PushConstant
    {
        VkDeviceAddress vertexBufferAddress;
    };

    VkPipeline       meshPipeline;
    VkPipelineLayout meshPipelineLayout;
};