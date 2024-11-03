#include "Graphics/Vulkan/PipelineBuilder.h"
#include "Graphics/Vulkan/VkUtil.h"

ComputePipelineBuilder::ComputePipelineBuilder(VkPipelineLayout pipelineLayout) : pipelineLayout(pipelineLayout) {}

ComputePipelineBuilder &ComputePipelineBuilder::setShaderModule(VkShaderModule module)
{
    this->module = module;
    return (*this);
}

VkPipeline ComputePipelineBuilder::build(VkDevice device)
{
    const auto pipelineCreateInfo = VkComputePipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage =
            {
                .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage  = VK_SHADER_STAGE_COMPUTE_BIT,
                .module = module,
                .pName  = "main",
            },
        .layout = pipelineLayout,
    };

    VkPipeline pipeline;
    VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline));

    return pipeline;
}
