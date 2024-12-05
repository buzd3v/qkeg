#pragma once

#include "Mesh/MeshDrawProps.h"
#include "Mesh/MeshPool.h"
class GPUDevice;
class MeshPipeline
{
  public:
    void init(GPUDevice &device, VkFormat colorImageFormat, VkFormat depthImageFormat,
              VkSampleCountFlagBits sampleCounts);
    void draw(VkCommandBuffer cmd, GPUDevice &device, VkExtent2D renderView, std::span<MeshId> meshList,
              std::span<MeshDrawProps> &drawProps);
    void cleanUp(VkDevice device);

  private:
    struct PushConstant
    {
        VkDeviceAddress vertexBufferAddress;
        glm::mat4       transform;
        uint32_t        materialId;
        uint32_t        padding;
    };

    VkPipeline       meshPipeline;
    VkPipelineLayout meshPipelineLayout;
};