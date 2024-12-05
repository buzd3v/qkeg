#pragma once

#include "GPUDevice.h"
#include <filesystem>
#include <stb/stb_image.h>
#include <vulkan/vulkan.h>

struct GPUImage;

struct RawImage
{
    RawImage() = default;
    ~RawImage();

    // move
    RawImage(RawImage &&props);
    RawImage &operator=(RawImage &&props);

    // not allow copy operation
    RawImage(RawImage &props)            = delete;
    RawImage &operator=(RawImage &props) = delete;

    unsigned char *pixels{nullptr};
    int            width{0};
    int            height{0};
    int            channels{0};

    bool shouldFreeData{false};
};

class ImageStream
{
  private:
    GPUDevice &device;
    int        getChannels(VkFormat imgFormat);

  public:
    void uploadDataToGPUImage(const GPUImage &image, void *pixels, uint32_t layers = 0);
    ImageStream(GPUDevice &device) : device(device) {}
    [[nodiscard]] RawImage loadRawImage(const std::filesystem::path &path);
    [[nodiscard]] GPUImage loadGPUImage(RawImage rawImage, VkFormat format, VkImageUsageFlags flags, bool mipmap);
    [[nodiscard]] GPUImage createGPUImage(GPUImageCreateInfo                     createInfo,
                                          std::optional<VmaAllocationCreateInfo> customAllocInfo = std::nullopt);
};