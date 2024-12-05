#include "Pipeline/MeshPipeline.h"

#include "GPUDevice.h"
#include "Mesh/MeshPool.h"
#include "Vulkan/PipelineBuilder.h"

void MeshPipeline::init(GPUDevice &device, VkFormat colorImageFormat, VkFormat depthImageFormat,
                        VkSampleCountFlagBits sampleCounts)
{
    auto vertexShader   = VkUtil::loadShaderModule("Shaders/mesh.vert.spv", device.getDevice());
    auto fragmentShader = VkUtil::loadShaderModule("Shaders/mesh.frag.spv", device.getDevice());

    const auto &pushConstant = VkPushConstantRange{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset     = 0,
        .size       = sizeof(PushConstant),
    };

    auto *bindlessSet   = BindlessDescriptor::GetInstance();
    auto  layouts       = std::array{bindlessSet->getDesSetLayout()};
    auto  pushConstants = std::array<VkPushConstantRange, 1>({pushConstant});
    meshPipelineLayout  = VkUtil::createPipelineLayout(device.getDevice(), layouts, pushConstants);

    meshPipeline = GraphicsPipelineBuilder{meshPipelineLayout}
                       .setShaderModule(vertexShader, fragmentShader)
                       .setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .setPolygonMode(VK_POLYGON_MODE_FILL)
                       .setMultiSampling(sampleCounts)
                       .enableCulling(true)
                       .setColorAttachmentFormat(colorImageFormat)
                       .setDepthFormat(depthImageFormat)
                       .enableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL)
                       .disableBlending()
                       .build(device.getDevice());

    vkDestroyShaderModule(device.getDevice(), vertexShader, nullptr);
    vkDestroyShaderModule(device.getDevice(), fragmentShader, nullptr);
}

void MeshPipeline::draw(VkCommandBuffer cmd, GPUDevice &device, VkExtent2D renderView, std::span<MeshId> meshList,
                        std::span<MeshDrawProps> &drawProps)
{
    auto bindlessSet = BindlessDescriptor::GetInstance();
    auto meshPool    = MeshPool::GetInstance();

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline);

    const auto viewport = VkViewport{
        .x        = 0,
        .y        = 0,
        .width    = (float)renderView.width,
        .height   = (float)renderView.height,
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    const auto scissor = VkRect2D{
        .offset = {},
        .extent = renderView,
    };
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    for (auto drawProp : drawProps)
    {
        // Draw mesh
        auto mesh = meshPool->getMesh(drawProp.meshID);
        vkCmdBindIndexBuffer(cmd, mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        // Submit push constants
        const auto pushConstant = PushConstant{
            .vertexBufferAddress = mesh.vertexBuffer.address,
            .transform           = drawProp.transform,
            .materialId          = drawProp.materialID,
        };

        vkCmdPushConstants(cmd,
                           meshPipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(PushConstant),
                           &pushConstant);
        vkCmdDrawIndexed(cmd, mesh.numIndices, 1, 0, 0, 0);
    }
}

void MeshPipeline::cleanUp(VkDevice device)
{
    vkDestroyPipelineLayout(device, meshPipelineLayout, nullptr);
    vkDestroyPipeline(device, meshPipeline, nullptr);
}
