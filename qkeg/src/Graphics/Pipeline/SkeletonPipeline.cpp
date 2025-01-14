#include "Pipeline/SkeletonPipeline.h"
#include "Mesh/MeshDrawProps.h"
#include "Mesh/MeshPool.h"
#include "Vulkan/GPUBuffer.h"
#include "Vulkan/PipelineBuilder.h"
void SkeletonPipeline::initialize(GPUDevice &device)
{
    const VkPushConstantRange pushConstantRange{
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset     = 0,
        .size       = sizeof(PushConstants),
    };

    // Create the pipeline layout with the defined push constant range.
    auto pushConstantRanges = std::array{pushConstantRange};
    skePipelineLayout       = VkUtil::createPipelineLayout(device.getDevice(), {}, pushConstantRanges);

    auto skeShader = VkUtil::loadShaderModule("shaders/skeleton.comp.spv", device.getDevice());

    skePipeline = ComputePipelineBuilder{skePipelineLayout}.setShaderModule(skeShader).build(device.getDevice());
    vkDestroyShaderModule(device.getDevice(), skeShader, nullptr);

    for (std::size_t frameIndex = 0; frameIndex < graphics::FRAMES_IN_FLIGHT; ++frameIndex)
    {
        auto &jointMatricesBuffer = perFrameResources[frameIndex].jointMatrices;

        jointMatricesBuffer.maxCapacity = MAX_JOINT_MATRICES;
        jointMatricesBuffer.gpuBuffer =
            GPUBufferBuilder(device, MAX_JOINT_MATRICES * sizeof(glm::mat4))
                .setBufferUsageFlags(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
                .build();
    }
}

void SkeletonPipeline::cleanUp(GPUDevice &device)
{
    for (std::size_t i = 0; i < graphics::FRAMES_IN_FLIGHT; ++i)
    {
        perFrameResources[i].jointMatrices.gpuBuffer.cleanup(device);
    }
    vkDestroyPipelineLayout(device.getDevice(), skePipelineLayout, nullptr);
    vkDestroyPipeline(device.getDevice(), skePipeline, nullptr);
}

void SkeletonPipeline::performSkinning(VkCommandBuffer commandBuffer, std::size_t frameIndex,
                                       const MeshDrawProps &drawCommand)
{
    auto meshPool = MeshPool::GetInstance();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, skePipeline);

    const auto &mesh = meshPool->getMesh(drawCommand.meshID);
    assert(mesh.hasSkeleton);
    assert(drawCommand.skinningMesh);

    const auto pushConstant = PushConstants{
        .jointMatricesBufferAddress = retrieveFrameResources(frameIndex).jointMatrices.gpuBuffer.address,
        .startJointMatrixIndex      = drawCommand.jointMatrixIndex,
        .vertexCount                = mesh.numVertices,
        .inputBufferAddress         = mesh.vertexBuffer.address,
        .skinningDataAddress        = mesh.skinningDataBuffer.address,
        .outputBufferAddress        = drawCommand.skinningMesh->skinnedVertexBuffer.address,
    };
    vkCmdPushConstants(
        commandBuffer, skePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstants), &pushConstant);

    static const auto workgroupSize = 256;
    const auto        groupSizeX    = (std::uint32_t)std::ceil(mesh.numVertices / (float)workgroupSize);
    vkCmdDispatch(commandBuffer, groupSizeX, 1, 1);
}

void SkeletonPipeline::startDrawing(std::size_t frameIndex)
{
    retrieveFrameResources(frameIndex).jointMatrices.reset();
}

std::size_t SkeletonPipeline::addJointMatrices(std::span<const glm::mat4> jointMatrices, std::size_t frameIndex)
{
    auto      &jointMatrix = retrieveFrameResources(frameIndex).jointMatrices;
    const auto startIndex  = jointMatrix.currentSize;
    jointMatrix.appendRange(jointMatrices);
    return startIndex;
}

SkeletonPipeline::FrameResources &SkeletonPipeline::retrieveFrameResources(std::size_t frameIndex)
{
    return perFrameResources[frameIndex];
}
