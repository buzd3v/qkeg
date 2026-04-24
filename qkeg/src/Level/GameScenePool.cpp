#include "Level/GameScenePool.h"

void GameScenePool::init(std::shared_ptr<GameScene> scene)
{
    if (!scene)
    {
        throw std::runtime_error("Cannot initialize GameScenePool with a null scene");
    }

    currentSceneName = scene->getSceneName();
    gameScenes[currentSceneName] = std::move(scene);
}

std::shared_ptr<GameScene> GameScenePool::getCurrentScene()
{
    if (auto it = gameScenes.find(currentSceneName); it != gameScenes.end())
    {
        return it->second;
    }

    throw std::runtime_error("Current game scene was not found");
}

void GameScenePool::pushGameScene(const std::string &name, std::shared_ptr<GameScene> scene)
{
    if (!scene)
    {
        throw std::runtime_error("Cannot push a null game scene");
    }

    gameScenes[name] = std::move(scene);
}

void GameScenePool::changeGameScene(const std::string &name)
{
    if (!gameScenes.contains(name))
    {
        throw std::runtime_error("Game scene was not registered: " + name);
    }

    currentSceneName = name;
}

void GameScenePool::deleteGameScene(const std::string &name)
{
    if (name == currentSceneName)
    {
        currentSceneName.clear();
    }

    gameScenes.erase(name);
}
