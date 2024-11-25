#pragma once
#include <glm/glm.hpp>
#include <singleton_atomic.hpp>

#include "Pipeline/MeshPipeline.h"
#include "Vulkan/GPUImage.h"

class MeshPool;
class GPUDevice;

class GameRenderer : public SingletonAtomic<GameRenderer>
{
  public:
    void init(GPUDevice &device, const glm::ivec2 &drawImageSize);
    void draw(GPUDevice &device, VkCommandBuffer cmd);
    void cleanUp(GPUDevice &device);

  public:
    GPUImage &getDrawImage(GPUDevice &device) const;
    GPUImage &getDepthImage(GPUDevice &device) const;
    VkFormat  getDrawImageFormat() const;
    VkFormat  getDepthImageFormat() const;

  private:
    void createDrawImage(GPUDevice &device, glm::ivec2 renderSize, bool haveCreate);

    MeshPipeline meshPipeline;

    ImageId drawImageId{qTypes::NULL_IMAGE_ID};
    ImageId depthImageId{qTypes::NULL_IMAGE_ID};
    ImageId resolveImageId{qTypes::NULL_IMAGE_ID};
    ImageId resolveDepthImageId{qTypes::NULL_IMAGE_ID};
    ImageId postFxImageId{qTypes::NULL_IMAGE_ID};

    VkFormat drawImageFormat{VK_FORMAT_R16G16B16A16_SFLOAT};
    VkFormat depthImageFormat{VK_FORMAT_D32_SFLOAT};

    VkSampleCountFlagBits sampleCounts{VK_SAMPLE_COUNT_1_BIT};
};
