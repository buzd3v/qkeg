#pragma once

// clang-format off
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format 

#include "Vulkan/Swapchain.h"
#include "Core/DeletionQueue.h"
#include "Vulkan/GPUImage.h"

struct GPUImage;
struct GLFWwindow;
struct Version
{
    int major;
    int minor;
    int patch;
    Version() : major(0), minor(0), patch(0) {};
    Version(int major, int minor, int patch) : major(major), minor(minor), patch(patch) {};
};

class GPUDevice
{
  public:
    static constexpr int FRAME_ON_FLIGHT = 2;
    struct FrameData
    {
      VkCommandPool commandPool;
      VkCommandBuffer mainCommandBuffer;
    };
  public:
    GPUDevice(){}
    GPUDevice(const GPUDevice&) = delete;
    GPUDevice& operator=(const GPUDevice&) = delete;
    void init(GLFWwindow* window, const char* appName, const Version& version);
    void cleanUp();

    VkCommandBuffer beginFrame();
    void endFrame(VkCommandBuffer& cmdBuf,const GPUImage& image);

  public:
    FrameData& getCurrentFrame() {return frames[frameNumber % FRAME_ON_FLIGHT];}
    std::uint32_t getCurrentFrameIndex() {return frameNumber % FRAME_ON_FLIGHT;} 
    bool needSwapchainRecreate() { return swapchain.needRecreate();}
    void recreateSwapchain(std::uint32_t width, std::uint32_t height);
    VkDevice getDevice(){return device.device;}

  private:
    void initVulkan(GLFWwindow *window, const char *appName, const Version &version);
    void createCommandBuffers();   
     
  private:
    vkb::Instance       instance;
    vkb::PhysicalDevice physicalDevice;
    vkb::Device         device;
    VmaAllocator        allocator;

    std::uint32_t graphicsQueueFamily;
    VkQueue     graphicsQueue;

    VkSurfaceKHR surface;
    VkFormat     swapchainFormat;
    Swapchain    swapchain;

    std::array<FrameData, Graphics::FRAME_ON_FLIGHT> frames;
    std::uint32_t frameNumber{0};

    DeletionQueue deletionQueue;


    //testing
    GPUImage _drawImage;
};
