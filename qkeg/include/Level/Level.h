#pragma once

#include "Common.h"
#include <filesystem>
// this will load a level from asset/levels/<level_name>.json
// properties of levels
// "model" -> gltf_file_path
// "background_music" -> bgm_file_path
//  "ambient"
// {
//     "color": [10,10,10],
//     "intensity": 100
// }
// "cubbemap" -> path_to_cubemap
// "default_camera" -> type of camera
class Level
{
  public:
    Level();
    void loadLevelFile(std::filesystem::path &path);
    void reset();

    std::string           levelName;
    std::filesystem::path levelPath;
    std::filesystem::path scenePath;
    std::filesystem::path bgmPath;
    std::filesystem::path cubemapPath;

    std::string cameraSpawn;

    qColor::LinearColor ambientColor;
    float               ambientIntensity;
};