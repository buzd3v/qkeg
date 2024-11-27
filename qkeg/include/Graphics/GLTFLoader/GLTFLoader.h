#pragma once
#include <filesystem>

struct Scene;
class GPUDevice;

namespace glTFUtil
{
Scene loadGltfFile(std::filesystem::path path, GPUDevice &device);
};