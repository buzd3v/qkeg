#include "ECS/EntityUtil.h"
#include "ECS/Component/Component.h"
#include "ECS/EntityLoader.h"
#include "Renderer/Scene.h"
void EnttUtil::addChild(entt::handle parent, entt::handle child)
{
    auto cHierachy = parent.get<HierarchyComponent>();
    auto it        = std::find(cHierachy.children.begin(), cHierachy.children.end(), child);

    if (it != cHierachy.children.end()) // alrready added
    {
        return;
    }
    auto &cChildHie = child.get<HierarchyComponent>();
    if (cChildHie.hasParent())
    {
        // remove old parent - child relationship
        auto &parent = cChildHie.parent.get<HierarchyComponent>();
        std::erase(parent.children, child);
    }

    cChildHie.parent = parent;
    cHierachy.children.push_back(child);
}
std::string fromCamelCaseToSnakeCase(const std::string &str)
{
    std::string res;
    bool        first = true;
    for (const auto &c : str)
    {
        if (std::isupper(c))
        {
            if (first)
            {
                first = false;
            }
            else
            {
                res += "_";
            }
            res += std::tolower(c);
        }
        else
        {
            res += c;
        }
    }
    return res;
}
std::string EnttUtil::getPrefabNameFromSceneNode(const EntityLoader &el, const SceneNode &node,
                                                 const std::string &defaultPrefabName)
{
    if (node.lightIndex != -1)
    {
        return "light";
    }
    if (node.cameraIndex != -1)
    {
        return "camera";
    }

    const auto nodeName = fromCamelCaseToSnakeCase(node.name);
    const auto dotPos   = nodeName.find_first_of(".");
    if (dotPos == std::string::npos)
    {

        if (const auto &name = el.getMappedPrefabName(nodeName); !name.empty())
        {
            return name;
        }
    }
    else
    {
        const auto substr = nodeName.substr(0, dotPos);
        if (const auto &name = el.getMappedPrefabName(substr); !name.empty())
        {
            return name;
        }
    }
    return defaultPrefabName;
}