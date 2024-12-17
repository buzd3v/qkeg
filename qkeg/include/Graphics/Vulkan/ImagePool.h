#pragma once
// #include "BindlessDescriptor.h"
#include "GPUImage.h"

#include <filesystem>
#include <singleton_dclp.hpp>
#include <string>
#include <unordered_map>

class GPUDevice;
class ImagePool : public SingletonDclp<ImagePool>
{
  public:
    ImagePool(GPUDevice &device);

    ImageId loadImageFromFile(std::filesystem::path &path, VkFormat format, VkImageUsageFlags flags, bool mipMap);

    // query an image with id
    const GPUImage &getImage(ImageId id) const;

    // a drawable null image
    [[nodiscard]] ImageId createImage(GPUImageCreateInfo &info, void *pixelData = nullptr,
                                      ImageId id = qTypes::NULL_IMAGE_ID);

    // request for a new ImageId
    [[nodiscard]] ImageId requestImageId() const;
    [[nodiscard]] ImageId getWhiteTextureID() { return whiteTextureId; }

    [[nodiscard]] ImageId loadCubemap(std::filesystem::path &path, std::array<std::string, 6> imgNames);
    // destroy all image
    void destroyAll();

    ImageId addImage(GPUImage image, ImageId = GPUImage::NULL_BINDLESS_ID);

  private:
  private:
    std::vector<GPUImage> images;
    std::vector<GPUImage> deletedImages;
    GPUDevice            &device;
    struct ImageProperties
    {
        std::filesystem::path path;
        VkFormat              format;
        VkImageUsageFlags     flags;
        bool                  mimap;

        bool operator==(ImageProperties &other)
        {
            return (path == other.path && format == other.format && flags == other.flags && mimap == other.mimap);
        }
    };
    std::unordered_map<ImageId, ImageProperties> imagesProperties;

    ImageId errorId{qTypes::NULL_IMAGE_ID};
    ImageId whiteTextureId{qTypes::NULL_IMAGE_ID};
};