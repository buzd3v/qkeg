#pragma once

// clang-format off
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format on

#include "Core/DeletionQueue.h"
#include "Graphics/Common.h"
#include "Vulkan/BindlessDescriptor.h"
#include "Vulkan/ImagePool.h"
#include "Vulkan/Swapchain.h"
#include "Vulkan/VkExcutor.h"

#include <glm/vec4.hpp>

struct GPUImage;
struct GLFWwindow;
struct Version
{
    int major;
    int minor;
    int patch;
    Version() : major(0), minor(0), patch(0){};
    Version(int major, int minor, int patch) : major(major), minor(minor), patch(patch){};
};

class GPUDevice
{
  public:
    static constexpr int FRAME_ON_FLIGHT = 2;
    struct FrameData
    {
        VkCommandPool   commandPool;
        VkCommandBuffer mainCommandBuffer;
    };

    struct FrameProperties
    {
        qColor::LinearColor clearColor{0.f, 0.f, 0.f, 1.f};
        bool                copyImageToSwapchain{true};
        glm::ivec4          blitRect{};
        bool                linearSampler{true}; // if false using nearest sampler
        bool                drawImGui{true};
    };

  public:
    GPUDevice() {}
    GPUDevice(const GPUDevice &)            = delete;
    GPUDevice &operator=(const GPUDevice &) = delete;

    void            init(GLFWwindow *window, const char *appName, const Version &version);
    void            cleanUp();
    VkCommandBuffer beginFrame();
    void            endFrame(VkCommandBuffer &cmdBuf, const GPUImage &image, FrameProperties frameProps);

  public:
    VkDevice      getDevice() { return device.device; }
    FrameData    &getCurrentFrame() { return frames[frameNumber % FRAME_ON_FLIGHT]; }
    std::uint32_t getCurrentFrameIndex() { return frameNumber % FRAME_ON_FLIGHT; }
    VmaAllocator &getGlobalAllocator() { return allocator; }
    void          recreateSwapchain(std::uint32_t width, std::uint32_t height);
    void          waitIdle() { vkDeviceWaitIdle(device); }
    bool          needSwapchainRecreate() { return swapchain.needRecreate(); }

    // images section
  public:
    [[nodiscard]] GPUImage queryImage(ImageId id);
    VkSampleCountFlagBits  getMaxSupportSampleCounts() const { return highestSampleCount; }

  private:
    void initVulkan(GLFWwindow *window, const char *appName, const Version &version);
    void initSingletonComponent();
    void queryDeviceCapabilities();
    void createCommandBuffers();

  private:
    vkb::Instance       instance;
    vkb::PhysicalDevice physicalDevice;
    vkb::Device         device;
    VmaAllocator        allocator;

    std::uint32_t graphicsQueueFamily;
    VkQueue       graphicsQueue;

    VkSurfaceKHR surface;
    VkFormat     swapchainFormat;
    Swapchain    swapchain;

    std::uint32_t         frameNumber{0};
    float                 maxAnisotropy{0};
    VkSampleCountFlagBits supportSamplesCount;
    VkSampleCountFlagBits highestSampleCount;

    std::array<FrameData, Graphics::FRAME_ON_FLIGHT> frames;

    DeletionQueue deletionQueue;

    // testing
};
