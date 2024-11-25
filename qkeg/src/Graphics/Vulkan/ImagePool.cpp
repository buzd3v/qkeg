
#include "Graphics/Vulkan/ImagePool.h"
#include "Vulkan/ImageStream.h"

ImagePool::ImagePool(GPUDevice &device) : device(device) {}

ImageId ImagePool::loadImageFromFile(std::filesystem::path &path, VkFormat format, VkImageUsageFlags flags, bool mipMap)
{
    for (const auto &[id, props] : imagesProperties)
    {
        if ((path == props.path && format == props.format && flags == props.flags && mipMap == props.mimap))
            return id;
    }

    ImageStream is(device);
    const auto  image = is.loadGPUImage(is.loadRawImage(path), format, flags, mipMap);
    if (image.isInitialized() && image.getImageId() == errorId)
    {
        return errorId;
    }
    const auto id = requestImageId();
    addImage(std::move(image), id);
    imagesProperties.emplace(id,
                             ImagePool::ImageProperties{
                                 .path   = path,
                                 .format = format,
                                 .flags  = flags,
                                 .mimap  = mipMap,
                             });

    return id;
}
ImageId ImagePool::addImage(GPUImage image, ImageId id)
{
    if (id == qTypes::NULL_IMAGE_ID)
    {
        id = requestImageId();
    }
    image.setImageId(id);
    if (id != images.size())
    {
        deletedImages.push_back(std::move(images[id]));
        images[id] = std::move(image);
    }
    else
    {
        images.emplace_back(std::move(image));
    }
    auto *bindlessSet = BindlessDescriptor::GetInstance();
    bindlessSet->addImage(device.getDevice(), id, image.imageView);

    return id;
}

const GPUImage &ImagePool::getImage(ImageId id) const
{
    return images.at(id);
}

ImageId ImagePool::createImage(GPUImageCreateInfo &info, ImageId id)
{
    ImageStream is(device);
    if (id == qTypes::NULL_IMAGE_ID)
    {
        id = requestImageId();
    }
    auto image = is.createGPUImage(info);

    return addImage(std::move(image), id);
}
[[nodiscard]] ImageId ImagePool::requestImageId() const
{
    return (ImageId)images.size();
}

void ImagePool::destroyAll()
{
    for (auto &image : images)
    {
        vkDestroyImageView(device.getDevice(), image.imageView, nullptr);
        vmaDestroyImage(device.getGlobalAllocator(), image.image, image.allocation);
    }
    for (auto &image : deletedImages)
    {
        vkDestroyImageView(device.getDevice(), image.imageView, nullptr);
        vmaDestroyImage(device.getGlobalAllocator(), image.image, image.allocation);
    }
    deletedImages.clear();
    images.clear();

    imagesProperties.clear();
}