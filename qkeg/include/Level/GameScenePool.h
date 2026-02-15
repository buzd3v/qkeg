#pragma once
#include "GameScene.h"
#include <memory>
#include <singleton_atomic.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>

class GameScenePool : public SingletonAtomic<GameScenePool>
{
  public:
    void                       init(std::shared_ptr<GameScene> scene);
    std::shared_ptr<GameScene> getCurrentScene();
    void                       pushGameScene(const std::string &name, std::shared_ptr<GameScene> scene);
    void                       changeGameScene(const std::string &name);
    void                       deleteGameScene(const std::string &name);

  private:
    std::unordered_map<std::string, std::shared_ptr<GameScene>> gameScenes;
    std::string                                                 currentSceneName;
};
