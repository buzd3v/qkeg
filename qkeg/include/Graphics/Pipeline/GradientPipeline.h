#pragma once
#include "Pipeline.h"
#include <glm/vec4.hpp>

#include <vulkan/vulkan.h>

class GradientPipeline : public Pipeline
{
  private:
    struct PushConstant
    {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };

  public:
    virtual void init(GPUDevice &device);
    virtual void cleanUp(GPUDevice &device);
    virtual void beginDraw(int frameIndex);

  private:
    VkPipelineLayout gradientPipelineLayout;
    VkPipeline       gradientPipeline;
};