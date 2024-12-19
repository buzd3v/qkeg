#include "GPUDevice.h"
#include "Graphics/vulkan/PipelineBuilder.h"
#include <Camera/Camera.h>
#include <Pipeline/CubemapPipeline.h>

void CubemapPipeline::initialize(GPUDevice &device, VkFormat colorImageFormat, VkFormat depthImageFormat,
                                 VkSampleCountFlagBits sampleCounts)
{
    const VkPushConstantRange pushConstant{
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset     = 0,
        .size       = sizeof(PushConstants),
    };

    auto *bindlessSet        = BindlessDescriptor::GetInstance();
    auto  layouts            = std::array{bindlessSet->getDesSetLayout()};
    auto  pushConstantsRange = std::array<VkPushConstantRange, 1>({pushConstant});

    cubemapPipelineLayout = VkUtil::createPipelineLayout(device.getDevice(), layouts, pushConstantsRange);
    const auto vertShader = VkUtil::loadShaderModule("shaders/default.vert.spv", device.getDevice());
    const auto fragShader = VkUtil::loadShaderModule("shaders/cubemap.frag.spv", device.getDevice());

    cubemapPipeline = GraphicsPipelineBuilder{cubemapPipelineLayout}
                          .setShaderModule(vertShader, fragShader)
                          .setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                          .setPolygonMode(VK_POLYGON_MODE_FILL)
                          .disableBlending()
                          .enableCulling(false)
                          .setMultiSampling(sampleCounts)
                          .setColorAttachmentFormat(colorImageFormat)
                          .setDepthFormat(depthImageFormat)
                          .enableDepthTest(false, VK_COMPARE_OP_EQUAL)
                          .build(device.getDevice());

    vkDestroyShaderModule(device.getDevice(), vertShader, nullptr);
    vkDestroyShaderModule(device.getDevice(), fragShader, nullptr);
}

void CubemapPipeline::cleanUp(GPUDevice &device)
{
    vkDestroyPipeline(device.getDevice(), cubemapPipeline, nullptr);
    vkDestroyPipelineLayout(device.getDevice(), cubemapPipelineLayout, nullptr);
}

void CubemapPipeline::draw(VkCommandBuffer cmd, GPUDevice &device, Camera &camera)
{
    auto *bindlessSet = BindlessDescriptor::GetInstance();

    if (cubemapTextureId == qTypes::NULL_IMAGE_ID)
    {
        return;
    }

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, cubemapPipeline);
    bindlessSet->bindBindlessSet(cmd, cubemapPipelineLayout, VK_PIPELINE_BIND_POINT_GRAPHICS);
    glm::mat4  mat = camera.getViewProjectionMatrix();
    const auto pcs = PushConstants{
        .inverseViewProjection = glm::inverse(mat),
        .cameraPos             = glm::vec4{camera.getPosititon(), 1.f},
        .textureId             = (std::uint32_t)0,
    };
    vkCmdPushConstants(cmd, cubemapPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants), &pcs);

    vkCmdDraw(cmd, 3, 1, 0, 0);
}

void CubemapPipeline::setCubemap(const ImageId cubemapId)
{
    cubemapTextureId = cubemapId;
}
