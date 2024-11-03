#include "Graphics/Pipeline/GradientPipeline.h"
#include "Graphics/GPUDevice.h"
#include "Graphics/Vulkan/VkUtil.h"
#include "Graphics/vulkan/PipelineBuilder.h"
// #include "GradientPipeline.h"

void GradientPipeline::init(GPUDevice &gdevice)
{
    const auto &device       = gdevice.getDevice();
    const auto &pushConstant = VkPushConstantRange{
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset     = 0,
        .size       = sizeof(PushConstant),
    };

    auto pushConstants     = std::array<VkPushConstantRange, 1>({pushConstant});
    gradientPipelineLayout = VkUtil::createPipelineLayout(device, {}, pushConstants);

    const auto shader = VkUtil::loadShaderModule("Shaders/gradient.comp.spv", device);

    gradientPipeline = ComputePipelineBuilder{gradientPipelineLayout}.setShaderModule(shader).build(device);
    vkDestroyShaderModule(device, shader, nullptr);
}