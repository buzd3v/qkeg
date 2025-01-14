#include "GameScene.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

class GameScenePool
{
  public:
    void init(std::shared_ptr<GameScene> scene)
    {
        gameScenes[scene->getSceneName()] = scene;
        currentSceneName                  = scene->getSceneName();
    }

    std::shared_ptr<GameScene> getCurrentScene()
    {
        if (currentSceneName.empty() || gameScenes.find(currentSceneName) == gameScenes.end())
        {
            throw std::runtime_error("No current scene set or current scene not found.");
        }
        return gameScenes[currentSceneName];
    }

    void pushGameScene(const std::string &name, std::shared_ptr<GameScene> scene)
    {
        if (gameScenes.find(name) != gameScenes.end())
        {
            throw std::runtime_error("Scene with the given name already exists.");
        }
        gameScenes[name] = scene;
    }

    void changeGameScene(const std::string &name)
    {
        if (gameScenes.find(name) == gameScenes.end())
        {
            throw std::runtime_error("Scene with the given name does not exist.");
        }
        currentSceneName = name;
    }

    void deleteGameScene(const std::string &name)
    {
        if (gameScenes.find(name) == gameScenes.end())
        {
            throw std::runtime_error("Scene with the given name does not exist.");
        }
        if (name == currentSceneName)
        {
            throw std::runtime_error("Cannot delete the currently active scene.");
        }
        gameScenes.erase(name);
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<GameScene>> gameScenes;
    std::string                                                 currentSceneName;
};
