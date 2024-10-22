#include "Game.h"
#include "JsonParser/JsonFile.h"

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
    params.renderSize = {800, 600};
    rootNode.getIfExists("renderResolution", params.renderSize);
    rootNode.getIfExists("appName", params.appName);
}

void Game::customInit()
{
    inputManager.loadMapping("assets/settings/input_actions.json", "assets/settings/input_mapping.json");
}

void Game::customUpdate(float dt) {}

void Game::customDraw() {}

void Game::customCleanup() {}
