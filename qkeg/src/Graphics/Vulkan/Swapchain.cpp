#include "Graphics/Vulkan/Swapchain.h"

#define VOLK_IMPLEMENTATION

#include <algorithm>
#include <fmt/format.h>
#include <vulkan/vk_enum_string_helper.h>

SwapchainSupportDetails Swapchain::querySwapchainSupport(const vkb::PhysicalDevice &physDevice, VkSurfaceKHR &surface)
{
    VkPhysicalDevice        device = physDevice.physical_device;
    SwapchainSupportDetails swapchainSupport;
    // query capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &swapchainSupport.capabilities);

    // query surface formats
    uint32_t formatCounts;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCounts, nullptr);
    swapchainSupport.formats.resize(formatCounts);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCounts, swapchainSupport.formats.data());

    uint32_t modeCounts;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &modeCounts, nullptr);
    swapchainSupport.presentModes.resize(formatCounts);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &modeCounts, swapchainSupport.presentModes.data());

    return swapchainSupport;
}

VkExtent2D Swapchain::chooseSwapExtend(SwapchainSupportDetails &swapchainSupport, int width, int height) const
{
    const auto capabilities = swapchainSupport.capabilities;
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkPresentModeKHR Swapchain::choosePresentMode(SwapchainSupportDetails &swapchainSupport) const
{
    const auto       availablePresentModes = swapchainSupport.presentModes;
    VkPresentModeKHR bestMode              = VK_PRESENT_MODE_FIFO_KHR;

    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return bestMode;
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(SwapchainSupportDetails &swapchainSupport) const
{
    const auto availableFormats = swapchainSupport.formats;
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        return VkSurfaceFormatKHR{
            VK_FORMAT_B8G8R8A8_SNORM,
            VK_COLORSPACE_SRGB_NONLINEAR_KHR,
        };
    }
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

void Swapchain::create(const vkb::PhysicalDevice &physDevice, const vkb::Device &device, VkSurfaceKHR &surface,
                       int width, int height)
{
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physDevice, surface);
    const auto              result           = vkb::SwapchainBuilder{device}
                            .set_desired_format(chooseSurfaceFormat(swapchainSupport))
                            .set_desired_present_mode(choosePresentMode(swapchainSupport))
                            .set_desired_extent(chooseSwapExtend(swapchainSupport, width, height).width,
                                                chooseSwapExtend(swapchainSupport, width, height).height)
                            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                            .build();

    if (!result.has_value())
    {
        throw std::runtime_error(fmt::format("failed to create swapchain: error = {}, vk result = {}",
                                             result.full_error().type.message(),
                                             string_VkResult(result.full_error().vk_result)));
    };
    swapchain = result.value();

    images     = swapchain.get_images().value();
    imageViews = swapchain.get_image_views().value();
}

void Swapchain::recreate(const vkb::PhysicalDevice &physDevice, const vkb::Device &device, VkSurfaceKHR &surface,
                         int width, int height)
{
    assert(swapchain && "Swapchain do not exist!");
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physDevice, surface);

    auto result = vkb::SwapchainBuilder{device}
                      .set_old_swapchain(swapchain)
                      .set_desired_format(chooseSurfaceFormat(swapchainSupport))
                      .set_desired_present_mode(swapchain.present_mode)
                      .set_desired_extent(chooseSwapExtend(swapchainSupport, width, height).width,
                                          chooseSwapExtend(swapchainSupport, width, height).height)
                      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                      .build();

    if (!result.has_value())
    {
        throw std::runtime_error(fmt::format("failed to recreate swapchain: error = {}, vk result = {}",
                                             result.full_error().type.message(),
                                             string_VkResult(result.full_error().vk_result)));
    };

    vkb::destroy_swapchain(swapchain);
    for (auto imageView : imageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }

    swapchain  = result.value();
    images     = swapchain.get_images().value();
    imageViews = swapchain.get_image_views().value();

    shouldRecreate = false;
}

void Swapchain::cleanUp(VkDevice device)
{
    { // destroy images and swapchain
        for (auto &image : images)
        {
            vkDestroyImage(device, image, nullptr);
        }
        for (auto &imageview : imageViews)
        {
            vkDestroyImageView(device, imageview, nullptr);
        }
        vkb::destroy_swapchain(swapchain);
    }
}
