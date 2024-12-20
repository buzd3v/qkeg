#pragma once

#include <vector>

#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

struct HierarchyComponent
{
    entt::handle              parent{};
    std::vector<entt::handle> children;

    bool hasParent() const { return (bool)parent; }
};
