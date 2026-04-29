#pragma once

#include <filesystem>
#include <string>

#include <fmt/core.h>

namespace
{
std::string extractNameFromSceneNodeName(const std::string &sceneNodeName)
{
    if (sceneNodeName.empty())
    {
        return {};
    }
    const auto dotPos = sceneNodeName.find_last_of(".");
    if (dotPos == std::string::npos || dotPos == sceneNodeName.length() - 1)
    {
        fmt::println("cannot extract name from node '{}': should have format <prefab>.<name>", sceneNodeName);
    }
    return sceneNodeName.substr(dotPos + 1, sceneNodeName.size());
}
} // namespace

namespace GameUtil
{
template <typename F>
void foreachFileInDir(const std::filesystem::path &dir, F f)
{
    for (const auto &p : std::filesystem::recursive_directory_iterator(dir))
    {
        if (std::filesystem::is_regular_file(p))
        {
            f(p.path());
        }
    }
}

template <typename F>
void forEachFileInDir(const std::filesystem::path &dir, F f)
{
    foreachFileInDir(dir, f);
}

inline std::string capComponentNodeName(const std::string &sceneNodeName)
{
    return extractNameFromSceneNodeName(sceneNodeName);
}
} // namespace GameUtil
