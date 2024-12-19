#include "ECS/System/TransformSystem.h"
#include "ECS/Component/Component.h"
#include <glm/glm.hpp>

void updateEntity(entt::registry &reg, entt::entity entity, const glm::mat4 &parentTransform)
{
    auto [cTransform, cHierachy] = reg.get<TransformComponent, HierarchyComponent>(entity);
    if (!cHierachy.hasParent())
    {
        cTransform.worldTransform = cTransform.transform.getTransformMatrix();
    }
    else
    {
        auto parent               = cTransform.worldTransform;
        cTransform.worldTransform = parentTransform * cTransform.transform.getTransformMatrix();
        if (cTransform.worldTransform == parent) // local transform is I matrix
        {
            return;
        }
    }

    for (auto &child : cHierachy.children)
    {
        updateEntity(reg, child, cTransform.worldTransform);
    }
}

namespace TransformSystem
{
void update(entt::registry &registry, float /*deltaTime*/)
{
    for (auto &&[entity, cHierachy] : registry.view<HierarchyComponent>().each())
    {
        if (!cHierachy.hasParent())
        {
            updateEntity(registry, entity, qConstant::IDENTITY_MAT);
        }
    }
}
} // namespace TransformSystem