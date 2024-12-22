#include "Cubemap.h"
#include "GPUDevice.h"
#include "Vulkan/ImageStream.h"
#include <array>
GPUImage VkUtil::loadCubemap(GPUDevice &device, std::filesystem::path &path)
{
    GPUImage cubemapImg;

    const auto imgNames = std::array{
        "left.jpg",
        "right.jpg",
        "up.jpg",
        "down.jpg",
        "front.jpg",
        "back.jpg",
    };

    uint32_t    face      = 0;
    bool        isCreated = false;
    ImageStream stream(device);
    for (auto imgName : imgNames)
    {
        RawImage rawImg;
        if (std::filesystem::exists(path / imgName))
        {
            rawImg = stream.loadRawImage(path / imgName);
            assert(rawImg.channels == 4);
            assert(rawImg.pixels != nullptr);
        }
        if (!isCreated)
        {
            cubemapImg = stream.createGPUImage({
                .format      = VK_FORMAT_R8G8B8A8_SRGB,
                .usageFlags  = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                .createFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
                .extent =
                    {
                        .width  = (uint32_t)rawImg.width,
                        .height = (uint32_t)rawImg.height,
                        .depth  = 1,
                    },
                .numLayers = 6,
                .isCubemap = true,
            });
            isCreated  = true;
        }
        else
        {
        }
        stream.uploadDataToGPUImage(cubemapImg, rawImg.pixels, face);
        face++;
    }
    return cubemapImg;
}