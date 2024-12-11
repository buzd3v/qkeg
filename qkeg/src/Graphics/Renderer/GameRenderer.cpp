#include "Renderer/GameRenderer.h"
#include "Camera/Camera.h"
#include "GPUDevice.h"
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

    sceneDataBuffers.init(device,
                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                          graphics::FRAMES_IN_FLIGHT,
                          sizeof(SceneData));
}

void GameRenderer::draw(GPUDevice &device, VkCommandBuffer cmd, const Camera &camera, const SceneProps &props)
{

    { // upload scene data
        auto sceneData = SceneData{
            .view             = props.camera.getViewMatrix(),
            .projection       = props.camera.getProjection(),
            .viewProjection   = props.camera.getViewProjectionMatrix(),
            .ambientColor     = qColor::Color(props.ambientColor),
            .ambientIntensity = props.ambientIntensity,
            .materialAddress  = materialPool->getMaterialBuffer().address,
        };
        sceneDataBuffers.uploadData(cmd, device.getCurrentFrameIndex(), (void *)&sceneData, sizeof(SceneData));
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
        // TODO : complete deaw function
    }
}

void GameRenderer::cleanUp(GPUDevice &device)
{
    meshPool     = nullptr;
    materialPool = nullptr;

    sceneDataBuffers.cleanup(device);
    meshPipeline.cleanUp(device.getDevice());
}

void GameRenderer::addMesh(qTypes::MeshId meshId, qTypes::MaterialId matId, glm::mat4 &transform, MeshDrawProps props,
                           bool castShadow)
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
