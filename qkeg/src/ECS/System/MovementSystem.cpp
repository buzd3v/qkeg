#include "ECS/System/MovementSystem.h"
#include "ECS/Component/Component.h"

void MovementSystem::update(entt::registry &registry, float dt)
{
    for (auto &&[entity, transform, movement] : registry.view<TransformComponent, MovementComponent>().each())
    {
        movement.prevPos = glm::vec3(transform.worldTransform[3]);
        auto newPos      = transform.transform.getPosition() + movement.velocity * dt;
        transform.transform.setPosition(newPos);

        if (movement.rotationTime != 0.f)
        {
            movement.elapsedTime += dt;
            if (movement.elapsedTime >= movement.rotationTime)
            {
                transform.transform.setRotation(movement.targetRotation);
                movement.elapsedTime  = movement.rotationTime;
                movement.rotationTime = 0.f;
                continue;
            }
            auto newRot = glm::slerp(
                movement.startRotation, movement.targetRotation, movement.elapsedTime / movement.rotationTime);
            transform.transform.setRotation(newRot);
        }
    }
}