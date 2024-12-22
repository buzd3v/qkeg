#include "Level/Level.h"
#include "JsonParser/JsonFile.h"
Level::Level() : ambientColor(qConstant::DEFAUT_AMBIENT_COLOR), ambientIntensity(qConstant::DEFAULT_AMBIENT_INTENSITY)
{
}

void Level::loadLevelFile(std::filesystem::path &path)
{
    reset();
    levelPath = path;

    JsonFile file(levelPath);
    if (!file.isGood())
    {
        return;
    }

    const auto rtNode = file.getRootNode();
    rtNode.get("scene", scenePath);
    rtNode.getIfExists("background_music", bgmPath);
    rtNode.getIfExists("cubemap", cubemapPath);
    rtNode.getIfExists("default_camera", cameraSpawn);
    rtNode.getIfExists("default_spawn", playerSpawn);

    if (rtNode.hasKey("ambient"))
    {
        const auto &ambientNode = rtNode.getNode("ambient");
        ambientNode.get("color", ambientColor, qConstant::DEFAUT_AMBIENT_COLOR);
        ambientNode.get("intensity", ambientIntensity, qConstant::DEFAULT_AMBIENT_INTENSITY);
    }
}
void Level::reset()
{
    levelName.clear();
    levelPath.clear();
    scenePath.clear();
    bgmPath.clear();
    cameraSpawn.clear();
    cubemapPath.clear();
    ambientColor     = qConstant::DEFAUT_AMBIENT_COLOR;
    ambientIntensity = qConstant::DEFAULT_AMBIENT_INTENSITY;
}