#pragma once

#include "Skeleton.h"
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>

struct Animation
{
    struct JointTracks
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::quat> rotations;
        std::vector<glm::vec3> scales;
    };

    std::vector<JointTracks> jointTracks;

    float animationDuration{0.0f};
    bool  isLooping{true};
    int   startingFrame{0};

    std::string                             animationName;
    std::map<int, std::vector<std::string>> frameEvents;
    const std::vector<std::string>         &getEventsForFrame(int frame) const;
};

class Animator
{
  public:
    void assignAnimation(const Skeleton &skeleton, const Animation &animation);
    void update(const Skeleton &skeleton, float dt);

    const std::string getCurrentAnimName();
    void              setProgressNormalized(float normalizedTime);
    float             getProgressNormalized() const;

    const Animation *getCurrentAnimation() const { return currentAnimation; }

    bool                   isAnimFinish{false};
    bool                   isNextFrame{false};
    bool                   isFirstFrame{true};
    float                  currentTime{0.f};
    int                    currentFrame{0};
    std::vector<glm::mat4> jointMatricies;

  private:
    void computeJointMatricies(const Skeleton &skeleton);
    void computeJointTransform(const Skeleton &skeleton, qTypes::JointId id, const Animation &anim, float time,
                               const glm::mat4 parentTransform);

    void             resetAnimation(const Animation &animation);
    const Animation *currentAnimation{nullptr};
};