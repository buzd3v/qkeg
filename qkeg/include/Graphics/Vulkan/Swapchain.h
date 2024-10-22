#pragma once
// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
#include <VkBootstrap.h>
// clang-format on

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};
class Swapchain
{
  public:
    void create(const vkb::PhysicalDevice &physDevice, const vkb::Device &device, VkSurfaceKHR &surface, int width,
                int height);

    void recreate(const vkb::Device &device, VkFormat format, uint32_t width, uint32_t height);

    void cleanUp();

    // getter and setter
  public:
    VkExtent2D getExtent() { return swapchain.extent; }
    VkFormat   getImageFormat() { return swapchain.image_format; }

  private:
    SwapchainSupportDetails querySwapchainSupport(const vkb::PhysicalDevice &physDevice, VkSurfaceKHR &surface);
    VkExtent2D              chooseSwapExtend(SwapchainSupportDetails &swapchainSupport, int width, int height) const;
    VkSurfaceFormatKHR      chooseSurfaceFormat(SwapchainSupportDetails &swapchainSupport) const;
    VkPresentModeKHR        choosePresentMode(SwapchainSupportDetails &swapchainSupport) const;

  private:
    vkb::Swapchain swapchain{};

    std::vector<VkImage>     images;
    std::vector<VkImageView> imageViews;
};