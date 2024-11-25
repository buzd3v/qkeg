#pragma once
#include "Pipeline.h"
#include <glm/vec4.hpp>

#include <vulkan/vulkan.h>
struct GPUImage;
class GradientPipeline : public Pipeline
{
  private:
    struct PushConstant
    {
        std::uint32_t imageID;
    };

  public:
    virtual void init(GPUDevice &device);
    virtual void cleanUp(GPUDevice &device);
    virtual void beginDraw(int frameIndex) {}

    void draw(VkCommandBuffer cmd, GPUDevice &device, GPUImage &image);

  private:
    VkPipelineLayout gradientPipelineLayout;
    VkPipeline       gradientPipeline;
};