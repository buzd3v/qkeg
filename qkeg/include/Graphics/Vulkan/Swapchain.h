#pragma once
// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
#include <VkBootstrap.h>
// clang-format on
#include "Core/DeletionQueue.h"
#include "VkInitializer.h"
#include "VkUtil.h"

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
    void recreate(const vkb::PhysicalDevice &physDevice, const vkb::Device &device, VkSurfaceKHR &surface, int width,
                  int height);

    void beginFrame(VkDevice device, std::uint32_t frameIndex);
    void resetFence(VkDevice device, std::uint32_t frameIndex);
    void initSyncStructures(const VkDevice &device);

    std::pair<VkImage, std::uint32_t> acquireSwapchainImage(VkDevice &device, std::uint32_t frameIndex);
    void submitAndPresent(VkCommandBuffer &cmd, VkQueue &graphicsQueue, std::size_t frameIndex,
                          std::uint32_t swapchainImageIndex);

    void cleanUp(VkDevice device);
    // getter and setter
  public:
    VkExtent2D  getExtent() { return swapchain.extent; }
    VkFormat    getImageFormat() { return swapchain.image_format; }
    VkImageView getImageView(std::size_t imageIndex) { return imageViews[imageIndex]; }
    bool        needRecreate() const { return shouldRecreate; }

    const std::vector<VkImage> &getImages() { return images; }

  private:
    struct FrameData
    {
        VkSemaphore swapchainSemaphore, renderSemaphore;
        VkFence     renderFence;
    };

  private:
    SwapchainSupportDetails querySwapchainSupport(const vkb::PhysicalDevice &physDevice, VkSurfaceKHR &surface);
    VkExtent2D              chooseSwapExtend(SwapchainSupportDetails &swapchainSupport, int width, int height) const;
    VkSurfaceFormatKHR      chooseSurfaceFormat(SwapchainSupportDetails &swapchainSupport) const;
    VkPresentModeKHR        choosePresentMode(SwapchainSupportDetails &swapchainSupport) const;

  private:
    vkb::Swapchain           swapchain{};
    std::vector<VkImage>     images;
    std::vector<VkImageView> imageViews;
    bool                     shouldRecreate{false};

    std::array<FrameData, Graphics::FRAME_ON_FLIGHT> frames;

    DeletionQueue deletionQueue;
};
