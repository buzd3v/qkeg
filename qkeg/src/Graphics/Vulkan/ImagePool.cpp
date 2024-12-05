
#include "Graphics/Vulkan/ImagePool.h"
#include "Vulkan/ImageStream.h"

ImagePool::ImagePool(GPUDevice &device) : device(device)
{
    { // create white texture
        std::uint32_t      pixel      = 0xFFFFFFFF;
        GPUImageCreateInfo createInfo = {
            .format     = VK_FORMAT_R8G8B8A8_UNORM,
            .usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .extent     = VkExtent3D{1, 1, 1},
        };
        whiteTextureId = createImage(createInfo, &pixel, whiteTextureId);
    }
}

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

ImageId ImagePool::createImage(GPUImageCreateInfo &info, void *pixelData, ImageId id)
{
    ImageStream is(device);
    if (id == qTypes::NULL_IMAGE_ID)
    {
        id = requestImageId();
    }

    auto image = is.createGPUImage(info);
    if (pixelData)
    {
        is.uploadDataToGPUImage(image, pixelData);
    }
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