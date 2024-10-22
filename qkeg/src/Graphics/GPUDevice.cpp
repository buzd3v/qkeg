#define VOLK_IMPLEMENTATION

#include "Graphics/GPUDevice.h"
#include "Graphics/Vulkan/Util.h"

#include <GLFW/glfw3.h>

void GPUDevice::init(GLFWwindow *window, const char *appName, const Version &version)
{
    // initVulkan
    initVulkan(window, appName, version);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    swapchain.create(physicalDevice, device, surface, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
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

    const auto deviceFeatures12 = VkPhysicalDeviceVulkan12Features{};
    const auto deviceFeatures13 = VkPhysicalDeviceVulkan13Features{};

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
