#pragma once

#include <Math/Transform.h>

struct TransformComponent
{
    Transform transform;
    glm::mat4 worldTransform{1.f};
};
