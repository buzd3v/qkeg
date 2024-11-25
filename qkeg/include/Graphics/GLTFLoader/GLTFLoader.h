#pragma once
#include <filesystem>

class Scene;
class GPUDevice;

namespace glTFUtil
{
Scene loadGltfFile(std::filesystem::path path, GPUDevice &device);
};