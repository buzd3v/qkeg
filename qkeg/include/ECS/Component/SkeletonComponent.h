#pragma once

#include "Renderer/Animation.h"
#include "Renderer/Skeleton.h"
struct SkeletonComponent
{
    Skeleton                                          skeleton;
    std::vector<SkinningMesh>                         skinMeshes;
    Animator                                          animator;
    const std::unordered_map<std::string, Animation> *animations{nullptr};
    int                                               skinId{-1}; // reference to skin id from the glTF scene
};