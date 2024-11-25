#define STB_IMAGE_IMPLEMENTATION

#include "Vulkan/ImageStream.h"
#include "Vulkan/GPUBuffer.h"
#include "Vulkan/GPUImage.h"
#include "Vulkan/VkExcutor.h"
#include "Vulkan/VkUtil.h"

#include <utility>

RawImage::RawImage(RawImage &&props)
    : width(props.width), height(props.height), channels(props.channels), shouldFreeData(props.shouldFreeData)
{
    std::exchange(props.pixels, this->pixels);
}

RawImage &RawImage::operator=(RawImage &&props)
{
    if (this != &props)
    {
        delete (pixels);
        std::exchange(pixels, props.pixels);
        height         = props.height;
        width          = props.width;
        channels       = props.channels;
        shouldFreeData = shouldFreeData;
    }

    return *this;
}

RawImage::~RawImage()
{
    if (shouldFreeData)
    {
        stbi_image_free(pixels);
    }
}

RawImage ImageStream::loadRawImage(const std::filesystem::path &path)
{
    RawImage rawImage;

    rawImage.shouldFreeData = true;
    rawImage.pixels = stbi_load(path.string().c_str(), &rawImage.width, &rawImage.height, &rawImage.channels, 4);
    if (!rawImage.pixels)
    {
        fmt::println("Failed to load image from file {}", path.string());
    }
    rawImage.channels = 4;

    return rawImage;
}

GPUImage ImageStream::loadGPUImage(RawImage rawImage, VkFormat format, VkImageUsageFlags flags, bool mipmap)
{
    auto image = createGPUImage({
        .format     = format,
        .usageFlags = flags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .extent =
            {
                .width  = (std::uint32_t)rawImage.width,
                .height = (std::uint32_t)rawImage.height,
                .depth  = 1,
            },
        .mipMap = mipmap,
    });

    uploadDataToGPUImage(image, rawImage.pixels);
    return image;
}

GPUImage ImageStream::createGPUImage(GPUImageCreateInfo                     createInfo,
                                     std::optional<VmaAllocationCreateInfo> customAllocInfo)
{
    std::uint32_t mipLevels = 1;
    if (createInfo.mipMap)
    {
        const auto maxExtent = std::max(createInfo.extent.width, createInfo.extent.height);
        mipLevels            = (std::uint32_t)std::floor(std::log2(maxExtent)) + 1;
    }

    if (createInfo.isCubemap)
    {
        assert(createInfo.numLayers % 6 == 0);
        assert(!createInfo.mipMap);
        assert(createInfo.createFlags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    }

    auto imageInfo = VkImageCreateInfo{
        .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags       = createInfo.createFlags,
        .imageType   = VK_IMAGE_TYPE_2D,
        .format      = createInfo.format,
        .extent      = createInfo.extent,
        .mipLevels   = mipLevels,
        .arrayLayers = createInfo.numLayers,
        .samples     = createInfo.samples,
        .tiling      = createInfo.tiling,
        .usage       = createInfo.usageFlags,
    };

    VmaAllocationCreateInfo allocInfo;
    if (customAllocInfo.has_value())
    {
        allocInfo = customAllocInfo.value();
    }
    else
    {
        allocInfo = defaultImageAllocationInfo;
    }

    GPUImage image;
    image.format    = createInfo.format;
    image.usage     = createInfo.usageFlags;
    image.extent    = createInfo.extent;
    image.mipLevels = mipLevels;
    image.numLayers = createInfo.numLayers;
    image.isCubeMap = createInfo.isCubemap;

    VK_CHECK(
        vmaCreateImage(device.getGlobalAllocator(), &imageInfo, &allocInfo, &image.image, &image.allocation, nullptr));

    bool shouldCreateImageView = ((createInfo.usageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) != 0) ||
                                 ((createInfo.usageFlags & VK_IMAGE_USAGE_STORAGE_BIT) != 0) ||
                                 ((createInfo.usageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0) ||
                                 ((createInfo.usageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0);

    if (shouldCreateImageView)
    {
        VkImageAspectFlags aspectflag =
            (createInfo.format == VK_FORMAT_D32_SFLOAT) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        auto viewType = createInfo.numLayers == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        if (createInfo.isCubemap)
        {
            viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        }
        auto imageViewInfo = VkImageViewCreateInfo{
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image    = image.image,
            .viewType = viewType,
            .format   = createInfo.format,
            .subresourceRange =
                {
                    .aspectMask     = aspectflag,
                    .baseMipLevel   = 0,
                    .levelCount     = mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount     = createInfo.numLayers,
                },
        };
        VK_CHECK(vkCreateImageView(device.getDevice(), &imageViewInfo, nullptr, &image.imageView));
    }
    return image;
}

void ImageStream::uploadDataToGPUImage(const GPUImage &image, void *pixels, uint32_t layers)
{
    uint32_t    channels = getChannels(image.format);
    std::size_t dataSize = image.extent.depth * image.extent.width * image.extent.height * channels;

    auto uploadBuffer = GPUBufferBuilder(device, dataSize).build();
    memcpy(uploadBuffer.info.pMappedData, pixels, dataSize);
    auto *excutor = VkExcutor::GetInstance();
    excutor->submit([&](VkCommandBuffer cmd) {
        assert((image.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0 &&
               "Image needs to have VK_IMAGE_USAGE_TRANSFER_DST_BIT to upload data to it");
        VkUtil::transitionImage(cmd, image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        const auto copyRegion = VkBufferImageCopy{
            .bufferOffset      = 0,
            .bufferRowLength   = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel       = 0,
                    .baseArrayLayer = layers,
                    .layerCount     = 1,
                },
            .imageExtent = image.extent,
        };

        vkCmdCopyBufferToImage(
            cmd, uploadBuffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        if (image.mipLevels > 1)
        {
            assert((image.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0 &&
                   (image.usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0 &&
                   "Image needs to have VK_IMAGE_USAGE_TRANSFER_{DST,SRC}_BIT to generate mip maps");
            VkUtil::generateMipmaps(
                cmd, image.image, VkExtent2D{image.extent.width, image.extent.height}, image.mipLevels);
        }
        else
        {
            VkUtil::transitionImage(
                cmd, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    });
    uploadBuffer.cleanup(device);
}

int ImageStream::getChannels(VkFormat imgFormat)
{
    int channels = 4; // Default to 4 channels if not explicitly handled
    switch (imgFormat)
    {
    // Single-channel formats
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8_SNORM:
    case VK_FORMAT_R8_UINT:
    case VK_FORMAT_R8_SINT:
    case VK_FORMAT_R8_SRGB:
    case VK_FORMAT_R16_UNORM:
    case VK_FORMAT_R16_SNORM:
    case VK_FORMAT_R16_UINT:
    case VK_FORMAT_R16_SINT:
    case VK_FORMAT_R16_SFLOAT:
    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32_SINT:
    case VK_FORMAT_R32_SFLOAT:
        channels = 1;
        break;

    // Two-channel formats
    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8_SNORM:
    case VK_FORMAT_R8G8_UINT:
    case VK_FORMAT_R8G8_SINT:
    case VK_FORMAT_R8G8_SRGB:
    case VK_FORMAT_R16G16_UNORM:
    case VK_FORMAT_R16G16_SNORM:
    case VK_FORMAT_R16G16_UINT:
    case VK_FORMAT_R16G16_SINT:
    case VK_FORMAT_R16G16_SFLOAT:
    case VK_FORMAT_R32G32_UINT:
    case VK_FORMAT_R32G32_SINT:
    case VK_FORMAT_R32G32_SFLOAT:
        channels = 2;
        break;

    // Three-channel formats
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8_SNORM:
    case VK_FORMAT_R8G8B8_UINT:
    case VK_FORMAT_R8G8B8_SINT:
    case VK_FORMAT_R8G8B8_SRGB:
    case VK_FORMAT_B8G8R8_UNORM:
    case VK_FORMAT_B8G8R8_SNORM:
    case VK_FORMAT_B8G8R8_UINT:
    case VK_FORMAT_B8G8R8_SINT:
    case VK_FORMAT_B8G8R8_SRGB:
    case VK_FORMAT_R32G32B32_UINT:
    case VK_FORMAT_R32G32B32_SINT:
    case VK_FORMAT_R32G32B32_SFLOAT:
        channels = 3;
        break;

    // Four-channel formats
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SNORM:
    case VK_FORMAT_R8G8B8A8_UINT:
    case VK_FORMAT_R8G8B8A8_SINT:
    case VK_FORMAT_R8G8B8A8_SRGB:
    case VK_FORMAT_B8G8R8A8_UNORM:
    case VK_FORMAT_B8G8R8A8_SNORM:
    case VK_FORMAT_B8G8R8A8_UINT:
    case VK_FORMAT_B8G8R8A8_SINT:
    case VK_FORMAT_B8G8R8A8_SRGB:
    case VK_FORMAT_R16G16B16A16_UNORM:
    case VK_FORMAT_R16G16B16A16_SNORM:
    case VK_FORMAT_R16G16B16A16_UINT:
    case VK_FORMAT_R16G16B16A16_SINT:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
    case VK_FORMAT_R32G32B32A32_UINT:
    case VK_FORMAT_R32G32B32A32_SINT:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        channels = 4;
        break;

    // Depth formats (often treated as single-channel for depth data)
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        channels = 1; // Depth-only format, usually treated as a single channel
        break;

    // Depth-stencil formats (commonly treated as two channels: one for depth, one for stencil)
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        channels = 2; // Depth and stencil channels
        break;

    default:
        channels = 4; // Default case, assuming RGBA if format is unknown
        break;
    }

    return channels;
}