#include "Renderer/AnimationPool.h"
#include "JsonParser/JsonFile.h"

void AnimationPool::addAnimationFromGLTF(const std::filesystem::path &glPath, AniMap animMap)
{
    auto it = aniPropsMaps.find(glPath.string());
    if (it != aniPropsMaps.end())
    {
        for (auto &[name, node] : it->second)
        {
            auto &anim         = animMap.at(name);
            anim.isLooping     = node.isLoop;
            anim.startingFrame = node.startFrame;
            anim.frameEvents   = node.events;
        }
    }
    animations.emplace(glPath.string(), std::move(animMap));
}

const AniMap &AnimationPool::getAnimations(const std::filesystem::path &glPath) const
{
    return animations.at(glPath.string());
}

void AnimationPool::loadFromDirectory(std::filesystem::path &glPath)
{
    JsonFile file(glPath);
    if (!file.isGood())
    {
        fmt::println("cannot load file {}", glPath.string());
        return;
    }

    const auto node = file.getRootNode();
    for (auto &animNode : node.getVector())
    {
        std::string path;
        animNode.get("mesh_path", path);
        auto &animData = aniPropsMaps[path];
        for (auto &[name, childNode] : animNode.getNode("animations").getKeyValueMap())
        {
            AniProperties props;
            childNode.get("looped", props.isLoop, true);
            childNode.get("startFrame", props.startFrame, 0);
            if (childNode.hasKey("events"))
            {
                for (auto &eventNode : childNode.getNode("events").getVector())
                {
                    std::string name;
                    int         frame;
                    eventNode.get("name", name);
                    eventNode.get("frame", frame);
                    props.events[frame].push_back(std::move(name));
                }
            }
            animData.emplace(name, std::move(props));
        }
    }
}
