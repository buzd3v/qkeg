#pragma once
#include <filesystem>
#include "Vulkan/GPUImage.h"
class GPUDevice;

namespace VkUtil
{
GPUImage loadCubemap(GPUDevice &device, std::filesystem::path &path);
}