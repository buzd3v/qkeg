#include "Renderer/GameRenderer.h"
#include "Camera/Camera.h"
#include "GPUDevice.h"
#include "Math/Transform.h"
#include "Mesh/MaterialPool.h"
#include "Vulkan/ImagePool.h"
#include "Vulkan/VkUtil.h"
#include <glm/glm.hpp>
void GameRenderer::init(GPUDevice &device, const glm::ivec2 &drawImageSize)
{
    sampleCounts = device.getMaxSupportSampleCounts();
    createDrawImage(device, drawImageSize, false);

    // Query for pool
    meshPool     = MeshPool::GetInstance();
    materialPool = MaterialPool::GetInstance();
    imagePool    = ImagePool::GetInstance();

    // Pipeline creation
    meshPipeline.init(device, drawImageFormat, depthImageFormat, sampleCounts);
    skeletonPipeline.initialize(device);
    cubemapPipeline.initialize(device, drawImageFormat, depthImageFormat, sampleCounts);
    sceneDataBuffers.init(device,
                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                          graphics::FRAMES_IN_FLIGHT,
                          sizeof(SceneData));

    lightBuffers.init(device,
                      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                      graphics::FRAMES_IN_FLIGHT,
                      sizeof(LightProps) * qConstant::MAX_LIGHT);
}

void GameRenderer::draw(GPUDevice &device, VkCommandBuffer cmd, Camera &camera, const SceneProps &props)
{

    {     // skinning
        { // Sync reading from skinning buffers with new writes
            const VkMemoryBarrier2 memBarrier{
                .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                .srcStageMask  = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
                .dstStageMask  = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            };
            const VkDependencyInfo depInfo = VkDependencyInfo{
                .sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .memoryBarrierCount = 1,
                .pMemoryBarriers    = &memBarrier,
            };
            vkCmdPipelineBarrier2(cmd, &depInfo);
        }

        for (const auto &drawProp : meshDrawProps)
        {
            if (!drawProp.skinningMesh)
            {
                continue;
            }
            skeletonPipeline.performSkinning(cmd, device.getCurrentFrameIndex(), drawProp);
        }
        { // Sync skinning witj next pipeline
            const auto memoryBarrier = VkMemoryBarrier2{
                .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                .srcStageMask  = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT,
                .dstStageMask  = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT,
            };
            const auto dependencyInfo = VkDependencyInfo{
                .sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .memoryBarrierCount = 1,
                .pMemoryBarriers    = &memoryBarrier,
            };
            vkCmdPipelineBarrier2(cmd, &dependencyInfo);
        }
    }
    { // upload scene data
        auto sceneData = SceneData{
            .view             = props.camera.getViewMatrix(),
            .projection       = props.camera.getProjection(),
            .viewProjection   = props.camera.getViewProjectionMatrix(),
            .ambientColor     = qColor::Color(props.ambientColor),
            .ambientIntensity = props.ambientIntensity,

            .lightsAddress = lightBuffers.getBuffer().address,
            .lightCounts   = (uint32_t)lightProps.size(),
            .sunlightIndex = sunlightIndex,

            .materialAddress = materialPool->getMaterialBuffer().address,
        };
        sceneDataBuffers.uploadData(cmd, device.getCurrentFrameIndex(), (void *)&sceneData, sizeof(SceneData));
    }
    { // sending light data
        lightBuffers.uploadData(
            cmd, device.getCurrentFrameIndex(), (void *)lightProps.data(), sizeof(LightProps) * lightProps.size());
    }

    const auto &drawImage    = imagePool->getImage(drawImageId);
    const auto &resolveImage = imagePool->getImage(resolveImageId);
    const auto &depthImage   = imagePool->getImage(depthImageId);

    { // rendering mesh
        VkUtil::transitionImage(
            cmd, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VkUtil::transitionImage(
            cmd, depthImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        if (sampleCounts != VK_SAMPLE_COUNT_1_BIT) // multi sampling
        {
            VkUtil::transitionImage(
                cmd, resolveImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }

        auto renderInfo = VkUtil::createRenderingInfo({
            .renderExtent         = drawImage.getExtent2D(),
            .colorImageView       = drawImage.imageView,
            .depthImageView       = depthImage.imageView,
            .resolveImageView     = sampleCounts == VK_SAMPLE_COUNT_1_BIT ? VK_NULL_HANDLE : resolveImage.imageView,
            .colorImageClearValue = glm::ivec4{0.f, 0.f, 0.f, 1.f}, // clear black
            .depthImageClearValue = 0.f,
        });

        vkCmdBeginRendering(cmd, &renderInfo.renderingInfo);
        meshPipeline.draw(cmd, device, drawImage.getExtent2D(), camera, sceneDataBuffers.getBuffer(), meshDrawProps);

        // cubemapPipeline.draw(cmd, device, camera);
        vkCmdEndRendering(cmd);
    }
}

void GameRenderer::cleanUp(GPUDevice &device)
{
    meshPool     = nullptr;
    materialPool = nullptr;

    meshPipeline.cleanUp(device.getDevice());
    skeletonPipeline.cleanUp(device);
    cubemapPipeline.cleanUp(device);
    sceneDataBuffers.cleanup(device);
    lightBuffers.cleanup(device);
}

void GameRenderer::createDrawImage(GPUDevice &device, glm::ivec2 renderSize, bool haveCreated)
{
    auto pool   = ImagePool::GetInstance();
    auto extent = VkExtent3D{
        .width  = (uint32_t)renderSize.x,
        .height = (uint32_t)renderSize.y,
        .depth  = 1,
    };

    // colorImage create
    {
        auto imageInfo = GPUImageCreateInfo{
            .format     = drawImageFormat,
            .usageFlags = qConstant::defaultDrawImageUsage,
            .extent     = extent,
            .samples    = sampleCounts,
        };

        drawImageId = pool->createImage(imageInfo, nullptr, drawImageId);
        if (!haveCreated)
        {
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            postFxImageId     = pool->createImage(imageInfo);
        }
    }
    // resolve image, this image type create once so must to check if already have it
    {
        if (!haveCreated)
        {
            auto resolveImageInfo = GPUImageCreateInfo{
                .format     = VK_FORMAT_R16G16B16A16_SFLOAT,
                .usageFlags = qConstant::defaultResolveImageUsage,
                .extent     = extent,
            };

            resolveImageId = pool->createImage(resolveImageInfo);
        }
    }

    // depth image
    {
        auto depthInfo = GPUImageCreateInfo{
            .format     = depthImageFormat,
            .usageFlags = qConstant::defaultDepthImageUsage,
            .extent     = extent,
            .samples    = sampleCounts,
        };
        depthImageId = pool->createImage(depthInfo, nullptr, depthImageId);
        if (!haveCreated)
        {
            depthInfo.samples   = VK_SAMPLE_COUNT_1_BIT;
            resolveDepthImageId = pool->createImage(depthInfo);
        }
    }
}

void GameRenderer::collectDataBegin(GPUDevice &device)
{
    // reset all the old data
    meshDrawProps.clear();
    lightProps.clear();
    sunlightIndex = -1;
    skeletonPipeline.startDrawing(device.getCurrentFrameIndex());
}

void GameRenderer::addLight(Light &light, Transform &transform)
{
    if (light.type == LightType::Directional)
    {
        sunlightIndex = (uint32_t)lightProps.size();
    }
    LightProps lp{
        .position    = transform.getPosition(),
        .type        = light.toType(),
        .direction   = transform.front(),
        .range       = light.range,
        .color       = qColor::Color(light.color),
        .intensity   = light.intensity,
        .scaleOffset = glm::vec2(light.scaleOffset),
        .shadowMap   = light.shadow,
    };
    if (lp.range == 0)
    {
        if (lp.type == qConstant::TYPE_LIGHT_POINT)
        {
            lp.range = qConstant::DEFAULT_POINT_LIGHT_RANGE;
        }
        else if (lp.type == qConstant::TYPE_LIGHT_SPOT)
        {
            lp.range = qConstant::DEFAULT_SPOT_LIGHT_RANGE;
        }
    }
    lightProps.push_back(lp);
}

void GameRenderer::addMesh(qTypes::MeshId meshId, qTypes::MaterialId matId, glm::mat4 &transform, bool castShadow)
{
    const auto &mesh     = meshPool->getMesh(meshId);
    const auto &material = materialPool->getMaterial(matId);

    meshDrawProps.push_back(MeshDrawProps{
        .transform  = transform,
        .materialID = matId,
        .meshID     = meshId,
        .shadow     = castShadow,
    });
}
void GameRenderer::addSkeletonMesh(qTypes::MeshId meshId, qTypes::MaterialId matId, glm::mat4 &transform,
                                   SkinningMesh &skinMesh, size_t jointMatStartIdx)
{
    const auto &mesh = meshPool->getMesh(meshId);

    meshDrawProps.push_back(MeshDrawProps{
        .transform        = transform,
        .materialID       = matId,
        .meshID           = meshId,
        .skinningMesh     = &skinMesh,
        .jointMatrixIndex = (uint32_t)jointMatStartIdx,
    });
}
std::size_t GameRenderer::addJointMatricies(GPUDevice &device, std::span<glm::mat4> jointMat)
{
    return skeletonPipeline.addJointMatrices(jointMat, device.getCurrentFrameIndex());
}

const GPUImage &GameRenderer::getDepthImage(GPUDevice &device)
{
    return ImagePool::GetInstance()->getImage(depthImageId);
}

const GPUImage &GameRenderer::getDrawImage(GPUDevice &device)
{
    return ImagePool::GetInstance()->getImage(drawImageId);
}

VkFormat GameRenderer::getDrawImageFormat() const
{
    return drawImageFormat;
}

VkFormat GameRenderer::getDepthImageFormat() const
{
    return depthImageFormat;
}
