#pragma once

#include "Mesh/MeshDrawProps.h"
#include "Mesh/MeshPool.h"
class GPUDevice;
class Camera;
class MeshPipeline
{
  public:
    void init(GPUDevice &device, VkFormat colorImageFormat, VkFormat depthImageFormat,
              VkSampleCountFlagBits sampleCounts);
    void draw(VkCommandBuffer cmd, GPUDevice &device, VkExtent2D renderView, const Camera &camera,
              const GPUBuffer &sceneData, std::vector<MeshDrawProps> &drawProps);
    void cleanUp(VkDevice device);

  private:
    struct PushConstant
    {
        VkDeviceAddress vertexBufferAddress;
        VkDeviceAddress sceneDataBuffer;
        glm::mat4       transform;
        uint32_t        materialId;
        uint32_t        padding;
    };

    VkPipeline       meshPipeline;
    VkPipelineLayout meshPipelineLayout;
};