#pragma once

#include "Animation.h"
#include <filesystem>
#include <singleton_atomic.hpp>
#include <unordered_map>

using AniMap = std::unordered_map<std::string, Animation>;

class AnimationPool : public SingletonAtomic<AnimationPool>
{
  public:
    void          loadFromDirectory(std::filesystem::path &path);
    void          addAnimationFromGLTF(const std::filesystem::path &glPath, AniMap anims);
    const AniMap &getAnimations(const std::filesystem::path &glPath) const;

  private:
    std::unordered_map<std::string, AniMap> animations;
    struct AniProperties
    {
        std::map<int, std::vector<std::string>> events;
        int                                     startFrame{0};
        bool                                    isLoop{true};
    };
    using AniPropsMap = std::unordered_map<std::string, AniProperties>;
    std::unordered_map<std::string, AniPropsMap> aniPropsMaps;
};