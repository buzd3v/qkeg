#include "Graphics/Pipeline/GradientPipeline.h"
#include "Graphics/GPUDevice.h"
#include "Graphics/Vulkan/VkUtil.h"
#include "Graphics/vulkan/PipelineBuilder.h"
#include "Vulkan/BindlessDescriptor.h"

void GradientPipeline::cleanUp(GPUDevice &device)
{
    vkDestroyPipelineLayout(device.getDevice(), gradientPipelineLayout, nullptr);
    vkDestroyPipeline(device.getDevice(), gradientPipeline, nullptr);
}

void GradientPipeline::init(GPUDevice &gdevice)
{
    const auto &device       = gdevice.getDevice();
    const auto &pushConstant = VkPushConstantRange{
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset     = 0,
        .size       = sizeof(PushConstant),
    };
    auto *bindlessSet = BindlessDescriptor::GetInstance();

    auto layouts           = std::array{bindlessSet->getDesSetLayout()};
    auto pushConstants     = std::array<VkPushConstantRange, 1>({pushConstant});
    gradientPipelineLayout = VkUtil::createPipelineLayout(device, layouts, pushConstants);

    const auto fragShader = VkUtil::loadShaderModule("Shaders/gradient.frag.spv", device);
    const auto vertShader = VkUtil::loadShaderModule("Shaders/fullScreenQuad.vert.spv", device);

    gradientPipeline = GraphicsPipelineBuilder{gradientPipelineLayout}
                           .setShaderModule(vertShader, fragShader)
                           .setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                           .setPolygonMode(VK_POLYGON_MODE_FILL)
                           .enableCulling(false)
                           .setMultiSampling(VK_SAMPLE_COUNT_1_BIT)
                           .enableBlending()
                           .setColorAttachmentFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
                           .disableDepthTest()
                           .build(gdevice.getDevice());

    vkDestroyShaderModule(device, fragShader, nullptr);
    vkDestroyShaderModule(device, vertShader, nullptr);
}

void GradientPipeline::draw(VkCommandBuffer cmd, GPUDevice &device, GPUImage &image)
{
    // auto *bindlessSet = BindlessDescriptor::GetInstance();

    // auto renderingInfo = VkUtil::createRenderingInfo({
    //     .renderExtent   = image.getExtent2D(),
    //     .colorImageView = image.imageView,
    // });

    // vkCmdBeginRendering(cmd, &renderingInfo.renderingInfo);
    // vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gradientPipeline);
    // bindlessSet->bindBindlessSet(cmd, gradientPipelineLayout, VK_PIPELINE_BIND_POINT_GRAPHICS);

    // const auto viewport = VkViewport{
    //     .x        = 0,
    //     .y        = 0,
    //     .width    = (float)image.getExtent2D().width,
    //     .height   = (float)image.getExtent2D().height,
    //     .minDepth = 0.f,
    //     .maxDepth = 1.f,
    // };
    // vkCmdSetViewport(cmd, 0, 1, &viewport);

    // const auto scissor = VkRect2D{
    //     .offset = {},
    //     .extent = image.getExtent2D(),
    // };
    // vkCmdSetScissor(cmd, 0, 1, &scissor);

    // const auto pcs = PushConstant{
    //     .imageID = image.getImageId(),
    // };
    // vkCmdPushConstants(cmd, gradientPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstant), &pcs);

    // vkCmdDraw(cmd, 3, 1, 0, 0);

    // vkCmdEndRendering(cmd);
}
