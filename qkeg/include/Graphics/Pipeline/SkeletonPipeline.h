#pragma once

#include "DynamicBuffer.h"
#include <array>
#include <glm/mat4x4.hpp>

struct MeshDrawProps;
class MeshCache;
class GPUDevice;

class SkeletonPipeline
{
  public:
    struct FrameResources
    {
        DynamicBuffer<glm::mat4> jointMatrices;
    };
    void initialize(GPUDevice &device);
    void cleanUp(GPUDevice &device);

    void performSkinning(VkCommandBuffer commandBuffer, std::size_t currentFrameIndex,
                         const MeshDrawProps &drawCommand);

    void startDrawing(std::size_t currentFrameIndex);

    std::size_t addJointMatrices(std::span<const glm::mat4> jointMatrices, std::size_t currentFrameIndex);

  private:
    VkPipelineLayout skePipelineLayout;
    VkPipeline       skePipeline;

    struct PushConstants
    {
        VkDeviceAddress jointMatricesBufferAddress;
        std::uint32_t   startJointMatrixIndex;
        std::uint32_t   vertexCount;
        VkDeviceAddress inputBufferAddress;
        VkDeviceAddress skinningDataAddress;
        VkDeviceAddress outputBufferAddress;
    };

    static constexpr std::size_t MAX_JOINT_MATRICES = 5000;

    std::array<FrameResources, graphics::FRAMES_IN_FLIGHT> perFrameResources;

    FrameResources &retrieveFrameResources(std::size_t currentFrameIndex);
};
