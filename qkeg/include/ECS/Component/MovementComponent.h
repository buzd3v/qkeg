#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

struct MovementComponent
{
    glm::vec3 prevFramePosition;
    glm::vec3 effectiveVelocity; // (newPos - prevPos) / dt

    glm::quat startHeading;
    glm::quat targetHeading;
    float     rotationTime{0.f};
    float     rotationProgress{0.f}; // from [0 to rotationTime] (used for slerp)
};
