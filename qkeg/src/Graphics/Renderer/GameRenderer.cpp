#include "Renderer/GameRenderer.h"
#include "GPUDevice.h"
#include "Vulkan/ImagePool.h"
#include "Vulkan/VkUtil.h"
void GameRenderer::init(GPUDevice &device, const glm::ivec2 &drawImageSize)
{
    sampleCounts = device.getMaxSupportSampleCounts();
    createDrawImage(device, drawImageSize, false);
    meshPipeline.init(device, drawImageFormat, depthImageFormat, sampleCounts);
}

void GameRenderer::draw(GPUDevice &device, VkCommandBuffer cmd) {}

void GameRenderer::cleanUp(GPUDevice &device) {}

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

        drawImageId = pool->createImage(imageInfo, drawImageId);
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
        depthImageId = pool->createImage(depthInfo, depthImageId);
        if (!haveCreated)
        {
            depthInfo.samples   = VK_SAMPLE_COUNT_1_BIT;
            resolveDepthImageId = pool->createImage(depthInfo);
        }
    }
}
