#include "Pipeline/SpritePipeline.h"
#include "Vulkan/PipelineBuilder.h"

void SpritePipeline::initialize(GPUDevice &device, VkFormat colorImageFormat, std::size_t maxSprites)
{
    const auto vertShader = VkUtil::loadShaderModule("Shaders/sprite.vert.spv", device.getDevice());
    const auto fragShader = VkUtil::loadShaderModule("Shaders/sprite.frag.spv", device.getDevice());

    if (!vertShader || !fragShader)
    {
        throw std::runtime_error("Cannot found sprite shader");
    }

    const auto &pushConstant = VkPushConstantRange{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset     = 0,
        .size       = sizeof(PushConstants),
    };

    auto *bindlessSet   = BindlessDescriptor::GetInstance();
    auto  layouts       = std::array{bindlessSet->getDesSetLayout()};
    auto  pushConstants = std::array<VkPushConstantRange, 1>({pushConstant});
    pipelineLayout      = VkUtil::createPipelineLayout(device.getDevice(), layouts, pushConstants);

    pipeline = GraphicsPipelineBuilder{pipelineLayout}
                   .setShaderModule(vertShader, fragShader)
                   .setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                   .setPolygonMode(VK_POLYGON_MODE_FILL)
                   .setMultiSampling(VK_SAMPLE_COUNT_1_BIT) // no sampling
                   .enableCulling(false)
                   .setColorAttachmentFormat(colorImageFormat)
                   .disableDepthTest()
                   .enableBlending()
                   .build(device.getDevice());

    vkDestroyShaderModule(device.getDevice(), vertShader, nullptr);
    vkDestroyShaderModule(device.getDevice(), fragShader, nullptr);

    for (std::size_t i = 0; i < graphics::FRAMES_IN_FLIGHT; i++)
    {
        auto &buffer = framesProps[i].spriteBuffers;
        buffer =
            GPUBufferBuilder{device, maxSprites * sizeof(SpriteDrawProps)}
                .setBufferUsageFlags(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
                .build();
    }
}

void SpritePipeline::cleanUp(GPUDevice &device)
{
    for (auto &buffers : framesProps)
    {
        buffers.spriteBuffers.cleanup(device);
    }

    vkDestroyPipeline(device.getDevice(), pipeline, nullptr);
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void SpritePipeline::draw(VkCommandBuffer cmd, GPUDevice &device, const GPUImage &drawImage, const glm::mat4 &viewProj,
                          const std::vector<SpriteDrawProps> &spriteDrawProps)
{
    auto bindlessSet = BindlessDescriptor::GetInstance();
    if (spriteDrawProps.empty())
    {
        return;
    }

    auto &currentBuf = framesProps[device.getCurrentFrameIndex()].spriteBuffers;

    memcpy(currentBuf.info.pMappedData, spriteDrawProps.data(), spriteDrawProps.size() * sizeof(SpriteDrawProps));

    auto renderInfor = VkUtil::createRenderingInfo({
        .renderExtent   = drawImage.getExtent2D(),
        .colorImageView = drawImage.imageView,
    });

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    bindlessSet->bindBindlessSet(cmd, pipelineLayout, VK_PIPELINE_BIND_POINT_GRAPHICS);

    vkCmdBeginRendering(cmd, &renderInfor.renderingInfo);

    const VkViewport viewPort{
        .x        = 0,
        .y        = 0,
        .width    = float(drawImage.getExtent2D().width),
        .height   = float(drawImage.getExtent2D().height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };
    const VkRect2D scissor{
        .offset = {},
        .extent = drawImage.getExtent2D(),
    };

    vkCmdSetViewport(cmd, 0, 1, &viewPort);
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    auto pushConstant = PushConstants{
        .viewProj       = viewProj,
        .commandsBuffer = currentBuf.address,
    };

    vkCmdPushConstants(cmd,
                       pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                       0,
                       sizeof(PushConstants),
                       &pushConstant);
    vkCmdDraw(cmd, 6, spriteDrawProps.size(), 0, 0);
    vkCmdEndRendering(cmd);
}
