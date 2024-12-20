#pragma once

#include <entt/entt.hpp>
class EntityLoader;
struct SceneNode;

namespace EnttUtil
{
void        addChild(entt::handle parent, entt::handle child); // add a child to the hierachy
std::string getPrefabNameFromSceneNode(const EntityLoader &el, const SceneNode &node,
                                       const std::string &defaultPrefabName);

} // namespace EnttUtil