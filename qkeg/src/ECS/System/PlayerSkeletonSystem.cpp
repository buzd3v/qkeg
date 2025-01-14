#include "ECS/System/PlayerSkeletonSystem.h"
#include "ECS/Component/Component.h"
#include "ECS/EntityUtil.h"
void PlayerSkeletonSystem::update(entt::handle player, float dt)
{
    auto &movementComponent = player.get<MovementComponent>();
    auto  velocity          = movementComponent.velocity;
    velocity.y              = 0.f;
    auto magnitude          = glm::length(velocity);
    bool isPlayerRunning;
    if (magnitude > 0.5f)
    {
        isPlayerRunning = true;
    }
    else
    {
        isPlayerRunning = false;
    }
    auto &animator = player.get<SkeletonComponent>().animator;
    if (std::abs(magnitude) <= 0.1f)
    {
        if (animator.getCurrentAnimName() == "Run" || animator.getCurrentAnimName() == "Walk" ||
            animator.getCurrentAnimName() == "Jump" || animator.getCurrentAnimName() == "Fall")
        {
            // eu::setAnimation(player, "Think");a
            EnttUtil::setAnimation(player, "Idle");
        }
    }
    else
    {
        if (isPlayerRunning)
        {
            EnttUtil::setAnimation(player, "Run");
        }
        else if (magnitude < 0.3f)
        {
            EnttUtil::setAnimation(player, "Walk");
        }
    }
}