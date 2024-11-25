#include "Graphics/Vulkan/PipelineBuilder.h"
#include "Graphics/Vulkan/VkUtil.h"
#include "Vulkan/VkInitializer.h"
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

GraphicsPipelineBuilder &GraphicsPipelineBuilder::disableDepthTest()
{
    depthStencil.depthTestEnable       = VK_FALSE;
    depthStencil.depthWriteEnable      = VK_FALSE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_NEVER;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable     = VK_FALSE;
    depthStencil.front                 = {};
    depthStencil.back                  = {};
    depthStencil.minDepthBounds        = 0.f;
    depthStencil.maxDepthBounds        = 1.f;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::enableDepthBias(float constantFactor, float slopeFactor)
{
    rasterizer.depthBiasEnable         = VK_TRUE;
    rasterizer.depthBiasConstantFactor = constantFactor;
    rasterizer.depthBiasSlopeFactor    = slopeFactor;

    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::enableDynamicDepth()
{
    dynamicDepth = true;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::enableDepthClamp()
{
    rasterizer.depthClampEnable = VK_TRUE;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::enableDepthTest(bool depthWriteEnable, VkCompareOp op)
{
    depthStencil.depthTestEnable       = VK_TRUE;
    depthStencil.depthWriteEnable      = depthWriteEnable;
    depthStencil.depthCompareOp        = op;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable     = VK_FALSE;
    depthStencil.front                 = {};
    depthStencil.back                  = {};
    depthStencil.minDepthBounds        = 0.f;
    depthStencil.maxDepthBounds        = 1.f;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::setDepthFormat(VkFormat format)
{
    renderInfo.depthAttachmentFormat = format;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::setColorAttachmentFormat(VkFormat format)
{
    colorAttachmentFormat              = format;
    renderInfo.colorAttachmentCount    = 1;
    renderInfo.pColorAttachmentFormats = &colorAttachmentFormat;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::enableBlending(VkBlendOp blendOp, VkBlendFactor srcBlendFactor,
                                                                 VkBlendFactor dstBlendFactor,
                                                                 VkBlendFactor srcAlphaBlendFactor,
                                                                 VkBlendFactor dstAlphaBlendFactor)
{
    colorBlendAttch.blendEnable = VK_TRUE;
    colorBlendAttch.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttch.srcColorBlendFactor = srcBlendFactor;
    colorBlendAttch.dstColorBlendFactor = dstBlendFactor;
    colorBlendAttch.srcAlphaBlendFactor = srcAlphaBlendFactor;
    colorBlendAttch.dstAlphaBlendFactor = dstAlphaBlendFactor;

    colorBlendAttch.colorBlendOp = blendOp;
    colorBlendAttch.alphaBlendOp = blendOp;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::disableBlending()
{
    colorBlendAttch.blendEnable = VK_FALSE;
    colorBlendAttch.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::setMultiSampling(VkSampleCountFlagBits flags)
{

    multiSampling.rasterizationSamples  = flags;
    multiSampling.sampleShadingEnable   = VK_FALSE;
    multiSampling.minSampleShading      = 1.f;
    multiSampling.alphaToCoverageEnable = VK_FALSE;
    multiSampling.alphaToOneEnable      = VK_FALSE;

    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::enableCulling(bool shouldEnable)
{
    if (shouldEnable)
    {
        return setCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    }
    else
    {
        return setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    }
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::setCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace)
{
    rasterizer.cullMode  = cullMode;
    rasterizer.frontFace = frontFace;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::setPolygonMode(VkPolygonMode mode)
{
    rasterizer.polygonMode = mode;
    rasterizer.lineWidth   = 1.f;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::setInputTopology(VkPrimitiveTopology topo)
{
    inputAsssembly.topology               = topo;
    inputAsssembly.primitiveRestartEnable = VK_FALSE;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::setShaderModule(VkShaderModule vertexShader,
                                                                  VkShaderModule fragmentShader,
                                                                  VkShaderModule geometryShader)
{
    shaderStages.clear();
    if (vertexShader && vertexShader != VK_NULL_HANDLE)
    {
        shaderStages.push_back(VkInitializer::shaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
    }
    if (geometryShader && geometryShader != VK_NULL_HANDLE)
    {
        shaderStages.push_back(VkInitializer::shaderStageCreateInfo(VK_SHADER_STAGE_GEOMETRY_BIT, geometryShader));
    }
    if (fragmentShader && fragmentShader != VK_NULL_HANDLE)
    {
        shaderStages.push_back(VkInitializer::shaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
    }
    return *this;
}

GraphicsPipelineBuilder::GraphicsPipelineBuilder(VkPipelineLayout pipelineLayout) : pipelineLayout(pipelineLayout)
{
    inputAsssembly  = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    rasterizer      = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    colorBlendAttch = {};
    multiSampling   = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    depthStencil    = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    renderInfo      = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
}

VkPipeline GraphicsPipelineBuilder::build(VkDevice device)
{
    VkPipelineViewportStateCreateInfo viewPort{
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount  = 1,
    };

    VkPipelineColorBlendStateCreateInfo colorBlend{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = &colorBlendAttch,
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    std::vector<VkDynamicState> dynamicState{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicstateInfo{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (uint32_t)dynamicState.size(),
        .pDynamicStates    = dynamicState.data(),
    };

    VkGraphicsPipelineCreateInfo createInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &renderInfo,
        .stageCount          = (std::uint32_t)shaderStages.size(),
        .pStages             = shaderStages.data(),
        .pVertexInputState   = &vertexInputInfo,
        .pInputAssemblyState = &inputAsssembly,
        .pViewportState      = &viewPort,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multiSampling,
        .pDepthStencilState  = &depthStencil,
        .pColorBlendState    = &colorBlend,
        .pDynamicState       = &dynamicstateInfo,
        .layout              = pipelineLayout,
    };

    VkPipeline pipeline;
    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline));

    return pipeline;
}