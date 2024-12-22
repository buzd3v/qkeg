#include "Renderer/ScenePool.h"
#include "GLTFLoader/GLTFLoader.h"
#include "Renderer/AnimationPool.h"
#include "Renderer/Scene.h"
ScenePool::ScenePool(GPUDevice &device) : device(device) {}

const Scene &ScenePool::addScene(const std::string &path, Scene scene)
{
    scene.path               = path;
    auto [it, insertedScene] = scenes.emplace(path, std::move(scene));
    return it->second;
}

const Scene &ScenePool::getScene(const std::string &path) const
{
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
    if (auto it = scenes.find(path); it != scenes.end())
    {
        return it->second;
    }

    auto scene = glTFUtil::loadGltfFile(std::filesystem::path(path), device);

    // return addScene(path, std::move(scene));

    if (!scene.animations.empty())
    {
        AnimationPool::GetInstance()->addAnimationFromGLTF(path, scene.animations);
    }
    const auto [it2, inserted] = scenes.emplace(path, std::move(scene));
    assert(inserted);
    return it2->second;
}

void ScenePool::cleanUp(GPUDevice &device) {}
