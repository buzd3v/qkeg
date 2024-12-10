#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#include "Graphics/GPUDevice.h"
#include "Graphics/Vulkan/VkUtil.h"

#include "GPUDevice.h"
#include "Graphics/Vulkan/VkInitializer.h"
#include <GLFW/glfw3.h>

#include "Mesh/MaterialPool.h"
#include "Mesh/MeshPool.h"
#include "Renderer/GameRenderer.h"

void GPUDevice::cleanUp()
{
    for (auto &frame : frames)
    {
        vkDestroyCommandPool(device, frame.commandPool, 0);
    }

    VkExcutor::GetInstance()->cleanup(device);
    BindlessDescriptor::GetInstance()->cleanUp(device);
    ImagePool::GetInstance()->destroyAll();

    swapchain.cleanUp(device);
    vmaDestroyAllocator(allocator);
    vkb::destroy_surface(instance, surface);
    vkb::destroy_device(device);
    vkb::destroy_instance(instance);
}

void GPUDevice::init(GLFWwindow *window, const char *appName, const Version &version)
{
    // initVulkan
    initVulkan(window, appName, version);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    swapchain.create(physicalDevice, device, surface, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    swapchain.initSyncStructures(device.device);
    createCommandBuffers();

    queryDeviceCapabilities();
    initSingletonComponent();
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
        .descriptorIndexing                           = true,
        .descriptorBindingSampledImageUpdateAfterBind = true,
        .descriptorBindingStorageImageUpdateAfterBind = true,
        .descriptorBindingPartiallyBound              = true,
        .descriptorBindingVariableDescriptorCount     = true,
        .runtimeDescriptorArray                       = true,
        .scalarBlockLayout                            = true,
        .bufferDeviceAddress                          = true,

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
void GPUDevice::initSingletonComponent()
{
    { // construct an immediately excutor, command once used by this class will be
        // excutte immediately
        VkExcutor::Construct();
        VkExcutor::GetInstance()->init(device, graphicsQueue, graphicsQueueFamily);
    }

    { // construct BindlessDescriptor
        BindlessDescriptor::Construct();
        BindlessDescriptor::GetInstance()->init(device, maxAnisotropy);
    }

    { // construct ImagePool -> for image management
        ImagePool::Construct(*this);
    }

    { // MeshPool
        MeshPool::Construct();
    }

    { // MaterialPool
        MaterialPool::Construct();
        MaterialPool::GetInstance()->init(*this);
    }

    { // Game Renderer
        GameRenderer::Construct();
        GameRenderer::GetInstance()->init(*this, {swapchain.getExtent().width, swapchain.getExtent().height});
    }
}
void GPUDevice::queryDeviceCapabilities()
{
    // check limits
    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(physicalDevice, &props);

    maxAnisotropy = props.limits.maxSamplerAnisotropy;

    { // store which sampling counts HW supports
        const auto counts = std::array{
            VK_SAMPLE_COUNT_1_BIT,
            VK_SAMPLE_COUNT_2_BIT,
            VK_SAMPLE_COUNT_4_BIT,
            VK_SAMPLE_COUNT_8_BIT,
            VK_SAMPLE_COUNT_16_BIT,
            VK_SAMPLE_COUNT_32_BIT,
            VK_SAMPLE_COUNT_64_BIT,
        };

        const auto supportedByDepthAndColor =
            props.limits.framebufferColorSampleCounts & props.limits.framebufferDepthSampleCounts;
        supportSamplesCount = {};
        for (const auto &count : counts)
        {
            if (supportedByDepthAndColor & count)
            {
                supportSamplesCount = (VkSampleCountFlagBits)(supportSamplesCount | count);
                highestSampleCount  = count;
            }
        }
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

void GPUDevice::endFrame(VkCommandBuffer &cmdBuf, const GPUImage &drawImage, FrameProperties frameProps)
{
    auto [image, imageIndex] = swapchain.acquireSwapchainImage(device.device, getCurrentFrameIndex());
    if (image == VK_NULL_HANDLE)
    {
        return;
    }
    swapchain.resetFence(device, getCurrentFrameIndex());
    VkImageLayout currentLayout;
    // re-formatting image layout
    {
        // reformatting swapchain image for available to write
        VkImageSubresourceRange clearRange = VkInitializer::imageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
        VkUtil::transitionImage(cmdBuf, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
        currentLayout = VK_IMAGE_LAYOUT_GENERAL;
        // make a clear color image
        VkClearColorValue clearValue{frameProps.clearColor.red,
                                     frameProps.clearColor.green,
                                     frameProps.clearColor.blue,
                                     frameProps.clearColor.alpha};

        // clear image
        vkCmdClearColorImage(cmdBuf, image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
    }

    // copy image to swapchain
    if (frameProps.copyImageToSwapchain)
    {
        // change drawImage and swapchainimage format to availlable to copy
        VkUtil::transitionImage(
            cmdBuf, drawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        VkUtil::transitionImage(cmdBuf, image, currentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        currentLayout      = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        const auto sampler = frameProps.linearSampler ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        //
        // copy to swapchainImage
        if (frameProps.blitRect != glm::ivec4{})
        {
            VkUtil::copyImageToImage(
                cmdBuf, drawImage.image, image, drawImage.getExtent2D(), swapchain.getExtent(), sampler);
        }
        else
        {
            VkUtil::copyImageToImage(
                cmdBuf, drawImage.image, image, drawImage.getExtent2D(), swapchain.getExtent(), sampler);
        }
    }

    // make the swapchain image into presentable mode
    VkUtil::transitionImage(cmdBuf, image, currentLayout, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    // VkUtil::transitionImage(
    //     cmdBuf, drawImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
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

GPUImage GPUDevice::queryImage(ImageId id)
{
    auto *pool = ImagePool::GetInstance();
    return pool->getImage(id);
}

void GPUDevice::recreateSwapchain(std::uint32_t width, std::uint32_t height)
{
    VK_CHECK(vkDeviceWaitIdle(device));
    swapchain.recreate(physicalDevice, device, surface, width, height);
}
