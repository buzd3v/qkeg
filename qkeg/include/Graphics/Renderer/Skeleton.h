#pragma once

#include "Math/Transform.h"
#include <limits>
#include <string>
#include <vector>
namespace qTypes
{
using JointId                = std::uint32_t;
static JointId NULL_JOINT_ID = std::numeric_limits<JointId>::max();
} // namespace qTypes

struct Joint
{
    qTypes::JointId id{qTypes::NULL_JOINT_ID};
    Transform       transform;
};

struct JointNode
{
    std::vector<qTypes::JointId> child;
};

struct Skeleton
{
    std::vector<Joint>       joints;
    std::vector<JointNode>   hierachy;
    std::vector<std::string> jointNames;

    std::vector<glm::mat4> inverseMatricies;
};