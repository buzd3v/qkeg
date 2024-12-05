#include "Renderer/ScenePool.h"
#include "GLTFLoader/GLTFLoader.h"
#include "Renderer/Scene.h"
ScenePool::ScenePool(GPUDevice &device) : device(device)
{
    // TODO: insert constructor here
}

const Scene &ScenePool::addScene(const std::string &path, Scene &scene)
{
    scene.path               = path;
    auto [it, insertedScene] = scenes.emplace(path, std::move(scene));
    return it->second;
}

const Scene &ScenePool::getScene(const std::string &path) const
{
    // TODO: insert return statement here
    if (auto it = scenes.find(path); it != scenes.end())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error(fmt::format("Scene {} was not loaded!", path));
    }
}

const Scene &ScenePool::loadScene(const std::string &path)
{
    // TODO: insert return statement here
    if (auto it = scenes.find(path); it != scenes.end())
    {
        return it->second;
    }

    auto scene = glTFUtil::loadGltfFile(std::filesystem::path(path), device);

    return addScene(path, scene);
}
