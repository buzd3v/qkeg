#pragma once

#include "Renderer/Scene.h"
#include <singleton_atomic.hpp>
#include <unordered_map>
class GPUDevice;

class ScenePool : public SingletonAtomic<ScenePool>
{
  public:
    ScenePool(GPUDevice &device);
    const Scene &addScene(const std::string &path, Scene scene);
    const Scene &getScene(const std::string &path) const;

    const Scene &loadScene(const std::string &path);

    void cleanUp(GPUDevice &device);

  private:
    std::unordered_map<std::string, Scene> scenes{};

    GPUDevice &device;
};