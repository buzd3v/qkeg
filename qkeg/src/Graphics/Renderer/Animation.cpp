#include "Renderer/Animation.h"
#include "Common.h"
#include <algorithm>
#include <glm/gtx/compatibility.hpp>

#include <tuple>
std::tuple<int, int, float> determineKeyFrameIndicies(size_t keyNums, float t)
{
    // keys are sampled by ANIMATION_FPS, so finding prev/next key is easy
    const std::size_t prevKey = std::min((std::size_t)std::floor(t * qConstant::ANIMATION_FPS), keyNums - 1);
    const std::size_t nextKey = std::min(prevKey + 1, keyNums - 1);

    float time{0.f};
    if (prevKey != nextKey)
    {
        time = t * qConstant::ANIMATION_FPS - (float)prevKey;
    }

    return {prevKey, nextKey, time};
}

glm::mat4 calculateJointTransform(const Animation &animation, qTypes::JointId id, float t)
{
    const auto &jointTrack = animation.jointTracks[id];

    auto indentityMat = qConstant::IDENTITY_MAT;
    { // pos
        const auto &positions = jointTrack.positions;
        if (!positions.empty())
        {
            const auto [prevKey, nextKey, time] = determineKeyFrameIndicies(positions.size(), t);
            const auto position                 = glm::lerp(positions[prevKey], positions[nextKey], time);
            indentityMat[3]                     = glm::vec4(position, 1.f);
        }
    }

    { // rotation
        const auto &rotations = jointTrack.rotations;
        if (!rotations.empty())
        {
            const auto [prevKey, nextKey, time] = determineKeyFrameIndicies(rotations.size(), t);
            const auto rotation                 = glm::slerp(rotations[prevKey], rotations[nextKey], time);
            indentityMat *= glm::mat4_cast(rotation);
        }
    }

    { // scale
        const auto &scales = jointTrack.scales;
        if (!scales.empty())
        {
            const auto [prevKey, nextKey, time] = determineKeyFrameIndicies(scales.size(), t);
            const auto scale                    = glm::lerp(scales[prevKey], scales[nextKey], time);
            indentityMat                        = glm::scale(indentityMat, scale);
        }
    }

    return indentityMat;
}

void Animator::assignAnimation(const Skeleton &skeleton, const Animation &animation)
{
    if (currentAnimation != nullptr && currentAnimation->animationName == animation.animationName)
    {
        return;
    }
    jointMatricies.resize(skeleton.joints.size());
    resetAnimation(animation);
    computeJointMatricies(skeleton);
}

void Animator::update(const Skeleton &skeleton, float dt)
{
    if (!currentAnimation || isAnimFinish)
    {
        return;
    }

    currentTime += dt;
    if (currentTime > currentAnimation->animationDuration)
    {
        if (currentAnimation->isLooping)
        {
            currentTime -= currentAnimation->animationDuration;
        }
        else
        {
            currentTime  = currentAnimation->animationDuration;
            isAnimFinish = true;
        }
    }

    auto nextFrame = (int)std::floor(currentTime * (float)qConstant::ANIMATION_FPS);
    isNextFrame    = nextFrame != currentFrame;
    currentFrame   = nextFrame;

    computeJointMatricies(skeleton);
}

const std::string Animator::getCurrentAnimName()
{
    return currentAnimation ? currentAnimation->animationName : std::string("");
}

void Animator::setProgressNormalized(float normalizedTime)
{
    currentTime = normalizedTime * currentAnimation->animationDuration;
}

float Animator::getProgressNormalized() const
{
    return currentAnimation ? currentTime / currentAnimation->animationDuration : 0.f;
}

void Animator::computeJointMatricies(const Skeleton &skeleton)
{
    computeJointTransform(skeleton, qConstant::ROOT_JOINT_ID, *currentAnimation, currentTime, qConstant::IDENTITY_MAT);
}

void Animator::computeJointTransform(const Skeleton &skeleton, qTypes::JointId id, const Animation &anim, float time,
                                     const glm::mat4 parentTransform)
{
    auto transform       = calculateJointTransform(anim, id, time);
    auto globalTransform = parentTransform * transform;
    jointMatricies[id]   = globalTransform * skeleton.inverseMatricies[id];

    for (auto child : skeleton.hierachy[id].child)
    {
        computeJointTransform(skeleton, child, anim, time, globalTransform);
    }
}

void Animator::resetAnimation(const Animation &animation)
{
    currentTime      = (float)(animation.startingFrame / qConstant::ANIMATION_FPS);
    isAnimFinish     = false;
    currentFrame     = 0;
    isNextFrame      = false;
    currentAnimation = &animation;
}
