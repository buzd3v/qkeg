#pragma once

#include <singleton_atomic.hpp>
#include <unordered_map>

class GPUDevice;
struct Scene;

class ScenePool : public SingletonAtomic<ScenePool>
{
  public:
    ScenePool(GPUDevice &device);
    const Scene &addScene(const std::string &path, Scene scene);
    const Scene &getScene(const std::string &path) const;

    const Scene &loadScene(const std::string &path);

  private:
    std::unordered_map<std::string, Scene> scenes;

    GPUDevice &device;
};