#include "Game.h"
#include <utility>

#include "Cubemap.h"
#include "ECS/Component/Component.h"
#include "JsonParser/JsonFile.h"

#include "ECS/System/System.h"
#include "Levels/EntryLevel.h"
#include "Sound/SoundPool.h"
Game::Game() : Application() {}

void Game::loadAppSetting()
{
    const std::filesystem::path settingsPath{"assets/settings/app_settings.json"};
    JsonFile                    file(settingsPath);

    if (!file.isGood())
    {
        throw(std::runtime_error(fmt::format("Cannot open file: {}", settingsPath.string())));
        return;
    }
    const JsonNode   rootNode = file.getRootNode();
    std::vector<int> a;
    params.renderSize = {1920, 1080};
    rootNode.getIfExists("renderResolution", params.renderSize);
    rootNode.getIfExists("appName", params.appName);
}

void Game::customInit()
{
    imagePool     = ImagePool::GetInstance();
    meshPool      = MeshPool::GetInstance();
    materialPool  = MaterialPool::GetInstance();
    scenePool     = ScenePool::GetInstance();
    animationPool = AnimationPool::GetInstance();

    SoundPool::Construct();

    inputManager->loadInputBinding("assets/settings/input_actions.json", "assets/settings/input_mapping.json");
    GameScenePool::Construct();
    gamePool = GameScenePool::GetInstance();

    std::shared_ptr<GameScene> entryScene = std::make_shared<EntryScene>();
    entryScene->init(gpuDevice, "entryScene", params.windowSize);
    gamePool->pushGameScene(entryScene->getSceneName(), entryScene);
    gamePool->changeGameScene(entryScene->getSceneName());
}

void Game::customUpdate(float dt)
{
    gamePool->getCurrentScene()->update(dt);
}

void Game::customDraw()
{
    gamePool->getCurrentScene()->draw();
}

void Game::customCleanup()
{
    renderer->cleanUp(gpuDevice);
}

void Game::initDrawObjects() {}

void Game::loadLevel(std::filesystem::path &path) {}