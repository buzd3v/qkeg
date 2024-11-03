#pragma once
#include "BindlessDescriptor.h"
#include "GPUImage.h"

#include <filesystem>
#include <string>
#include <unordered_map>

using ImageId = uint32_t;
class GPUDevice;
class ImagePool
{
  public:
    ImagePool(GPUDevice &device);
    void loadImageRaw(std::filesystem::path &path, VkFormat &format, VkImageUsageFlags flags, bool mipMap);

    ImageId addImage(GPUImage &image, ImageId = GPUImage::NULL_BINDLESS_ID);

    const GPUImage       &getImage(ImageId id) const;
    [[nodiscard]] ImageId requestImageId() const;
    void                  destroyAll();

  private:
    std::vector<GPUImage> images;
    GPUDevice            &device;

    struct ImageProperties
    {
        std::filesystem::path path;
        VkFormat              format;
        VkImageUsageFlags     flags;
        bool                  mimap;
    };
    std::unordered_map<ImageId, ImageProperties> imagesProperties;
};