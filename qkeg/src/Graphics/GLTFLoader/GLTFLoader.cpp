#include "GLTFLoader/GLTFLoader.h"

#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>

#include "GPUDevice.h"
#include "Renderer/Scene.h"

Scene glTFUtil::loadGltfFile(std::filesystem::path path, GPUDevice &device)
{
    // fastgltf::Parser parser;
    // auto             model = parser.
    fastgltf::Parser parser{};
    // auto             model

    return Scene{};
}