#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

struct MovementComponent
{
    glm::vec3 maxSpeed;
    glm::vec3 prevPos;
    glm::vec3 velocity; // (newPos - prevPos) / dt

    glm::quat startRotation;
    glm::quat targetRotation;
    float     rotationTime{0.f};
    float     elapsedTime{0.f}; // use this for smooth rotation
};
