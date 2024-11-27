#include <fastgltf/core.hpp>

#include "GLTFLoader/GLTFLoader.h"
#include "GPUDevice.h"
#include "Renderer/Scene.h"

fastgltf::Asset loadglTF(const std::filesystem::path &path)
{
    fmt::println("Start loading gltf {}", path.string());

    fastgltf::Parser parser{};
    auto             data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.error() != fastgltf::Error::None)
    {
        throw(std::runtime_error(fmt::format("Cannot load file: {}", path.c_str())));
    }
    return std::move(parser.loadGltf(data.get(), path.parent_path(), fastgltf::Options::None).get());
}

GPUMaterial loadMaterial(GPUDevice &device, const fastgltf::Asset &asset, const std::filesystem::path &fileDir,
                         const fastgltf::Material &material)
{
}

Scene glTFUtil::loadGltfFile(std::filesystem::path path, GPUDevice &device)
{
    fastgltf::Asset asset = loadglTF(path);

    return Scene{};
}