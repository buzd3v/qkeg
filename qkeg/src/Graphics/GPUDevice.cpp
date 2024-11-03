#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#include "Graphics/GPUDevice.h"
#include "Graphics/Vulkan/VkUtil.h"

#include "Graphics/Vulkan/VkInitializer.h"
#include <GLFW/glfw3.h>

void GPUDevice::cleanUp() {}

void GPUDevice::init(GLFWwindow *window, const char *appName, const Version &version)
{
    // initVulkan
    initVulkan(window, appName, version);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    swapchain.create(physicalDevice, device, surface, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    swapchain.initSyncStructures(device.device);
    createCommandBuffers();
}

void GPUDevice::initVulkan(GLFWwindow *window, const char *appName, const Version &version)
{
    VK_CHECK(volkInitialize());
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    instance = vkb::InstanceBuilder{}
                   .set_app_name(appName)
                   .set_app_version(version.major, version.minor, version.patch)
                   .request_validation_layers(true)
                   .use_default_debug_messenger()
                   .require_api_version(1, 3, 0)
                   .build()
                   .value();

    if (!instance)
    {
        throw(std::runtime_error("Cannot create VkInstance"));
    }

    volkLoadInstance(instance);

    // create surface
    //  init surface
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Faled to create vulkan surface");
    }

    const auto deviceFeatures = VkPhysicalDeviceFeatures{
        .imageCubeArray    = VK_TRUE,
        .geometryShader    = VK_TRUE,
        .depthClamp        = VK_TRUE,
        .samplerAnisotropy = VK_TRUE,
    };

    const auto deviceFeatures12 = VkPhysicalDeviceVulkan12Features{
        .descriptorIndexing  = true,
        .bufferDeviceAddress = true,
    };
    const auto deviceFeatures13 = VkPhysicalDeviceVulkan13Features{
        .synchronization2 = true,
        .dynamicRendering = true,
    };

    physicalDevice = vkb::PhysicalDeviceSelector{instance}
                         .set_minimum_version(1, 2)
                         .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                         .set_required_features(deviceFeatures)
                         .set_required_features_12(deviceFeatures12)
                         .set_required_features_13(deviceFeatures13)
                         .set_surface(surface)
                         .select()
                         .value();

    if (!physicalDevice)
    {
        throw(std::runtime_error("Cannot create Physical Device"));
    }

    device = vkb::DeviceBuilder{physicalDevice}.build().value();

    if (!device)
    {
        throw(std::runtime_error("Cannot create Logical Device"));
    }

    volkLoadDevice(device);

    graphicsQueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();
    graphicsQueue       = device.get_queue(vkb::QueueType::graphics).value();

    { // init vma
        const auto vulkanFunctions = VmaVulkanFunctions{
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr   = vkGetDeviceProcAddr,
        };

        const auto allocatorCreateInfo = VmaAllocatorCreateInfo{
            .flags            = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice   = physicalDevice,
            .device           = device,
            .pVulkanFunctions = &vulkanFunctions,
            .instance         = instance,
            .vulkanApiVersion = VK_API_VERSION_1_3,
        };

        vmaCreateAllocator(&allocatorCreateInfo, &allocator);
    }
}
VkCommandBuffer GPUDevice::beginFrame()
{
    swapchain.beginFrame(device, getCurrentFrameIndex());

    const auto &frame       = getCurrentFrame();
    const auto &cmdBuf      = frame.mainCommandBuffer;
    const auto  cmdbeginInf = VkInitializer::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkResetCommandBuffer(cmdBuf, 0));
    VK_CHECK(vkBeginCommandBuffer(cmdBuf, &cmdbeginInf));

    return cmdBuf;
}

void GPUDevice::endFrame(VkCommandBuffer &cmdBuf, const GPUImage &drawImage)
{
    auto [image, imageIndex] = swapchain.acquireSwapchainImage(device.device, getCurrentFrameIndex());
    if (image == VK_NULL_HANDLE)
    {
        return;
    }
    swapchain.resetFence(device, getCurrentFrameIndex());

    // re-formatting image layout
    {
        VkUtil::transitionImage(cmdBuf, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        // make a clear color image
        VkClearColorValue clearValue;

        float flash = std::abs(std::sin(frameNumber / 120.f));
        clearValue  = {{0.0f, 0.0f, flash, 1.0f}};

        VkImageSubresourceRange clearRange = VkInitializer::imageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
        // clear image
        vkCmdClearColorImage(cmdBuf, image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
    }
    // make the swapchain image into presentable mode
    VkUtil::transitionImage(cmdBuf, image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // end the command buffer
    VK_CHECK(vkEndCommandBuffer(cmdBuf));

    // present
    {
        swapchain.submitAndPresent(cmdBuf, graphicsQueue, getCurrentFrameIndex(), imageIndex);
    }

    // counting frame number
    frameNumber++;
}

void GPUDevice::createCommandBuffers()
{
    VkCommandPoolCreateInfo commandPoolInfo =
        VkInitializer::commandPoolCreateInfo(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (int i = 0; i < FRAME_ON_FLIGHT; i++)
    {
        VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frames[i].commandPool));
        const VkCommandBufferAllocateInfo cmdAllocInfo = VkInitializer::commandBufferAlocInfo(frames[i].commandPool, 1);
        VK_CHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &frames[i].mainCommandBuffer));
    }
}

void GPUDevice::recreateSwapchain(std::uint32_t width, std::uint32_t height)
{
    VK_CHECK(vkDeviceWaitIdle(device));
    swapchain.recreate(physicalDevice, device, surface, width, height);
}
