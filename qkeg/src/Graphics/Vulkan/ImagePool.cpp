
#include "Graphics/Vulkan/ImagePool.h"

ImagePool::ImagePool(GPUDevice &device) : device(device) {}
void    ImagePool::loadImageRaw(std::filesystem::path &path, VkFormat &format, VkImageUsageFlags flags, bool mipMap) {}
ImageId ImagePool::addImage(GPUImage &image, ImageId)
{
    return 0;
}
const GPUImage &ImagePool::getImage(ImageId id) const
{
    return GPUImage();
}
[[nodiscard]] ImageId ImagePool::requestImageId() const
{
    return 1;
}
void ImagePool::destroyAll() {}