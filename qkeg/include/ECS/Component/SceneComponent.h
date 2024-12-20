#pragma once
#include <string>

struct SceneComponent
{
    std::string sceneName;     // from prefab, have value if using prefab
    std::string sceneCompName; //
    std::string sceneNodeName; // sync with the node name in gltf
};