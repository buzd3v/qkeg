#pragma once

// clang-format off
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format 

#include "Vulkan/Swapchain.h"

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
    GPUDevice(){}
    GPUDevice(const GPUDevice&) = delete;
    GPUDevice& operator=(const GPUDevice&) = delete;

    void init(GLFWwindow* window, const char* appName, const Version& version);

  private:
    void initVulkan(GLFWwindow *window, const char *appName, const Version &version);

  private:
    vkb::Instance       instance;
    vkb::PhysicalDevice physicalDevice;
    vkb::Device         device;
    VmaAllocator        allocator;

    std::uint32_t graphicsQueueFamily;
    VkQueue     graphicsQueue;

    VkSurfaceKHR surface;
    VkFormat     swapchainFormat;
    Swapchain      swapchain;
};
