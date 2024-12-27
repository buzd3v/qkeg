#include "ECS/System/SkeletonSystem.h"
#include "ECS/Component/Component.h"

void SkeletonSystem::update(entt::registry &registry, float dt)
{
    for (auto &&[entity, skeleton] : registry.view<SkeletonComponent>().each())
    {
        skeleton.animator.update(skeleton.skeleton, dt);
    }
}