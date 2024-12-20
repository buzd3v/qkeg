#pragma once
#include <glm/glm.hpp>

#include "Vulkan/ImagePool.h"

class GPUDevice;
class Camera;

class CubemapPipeline
{
  public:
    void initialize(GPUDevice &device, VkFormat colorImageFormat, VkFormat depthImageFormat,
                    VkSampleCountFlagBits sampleCounts);

    void cleanUp(GPUDevice &device);

    void draw(VkCommandBuffer cmd, GPUDevice &device, Camera &camera);

    void setCubemap(const ImageId cubemapId);

  private:
    VkPipelineLayout cubemapPipelineLayout;
    VkPipeline       cubemapPipeline;

    ImageId cubemapTextureId{qTypes::NULL_IMAGE_ID};

    struct PushConstants
    {
        glm::mat4     inverseViewProjection;
        glm::vec4     cameraPos;
        std::uint32_t textureId;
    };
};