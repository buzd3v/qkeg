#pragma once

#include "ECS/Component/Component.h"
#include <entt/entt.hpp>
#include <string>
class EntityLoader;
struct SceneNode;

namespace EnttUtil
{
void        addChild(entt::handle parent, entt::handle child); // add a child to the hierachy
std::string getPrefabNameFromSceneNode(const EntityLoader &el, const SceneNode &node,
                                       const std::string &defaultPrefabName);

template <typename ComponentName>
entt::handle findEnttByName(entt::registry &reg, std::string name);

} // namespace EnttUtil

template <typename ComponentName>
entt::handle EnttUtil::findEnttByName(entt::registry &reg, std::string name)
{
    for (auto &entt : reg.view<ComponentName>())
    {
        if (auto nameComp = reg.try_get<NameComponent>(entt); nameComp && nameComp->name == name)
        {
            return {reg, entt};
        }
    }
    return {reg, entt::null};
}