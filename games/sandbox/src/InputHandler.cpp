#include "ECS/System/MovementSystem.h"
#include "ECS/System/System.h"
#include "Game.h"
#include "glm/glm.hpp"
void Game::handleInput(float dt)
{
    cameraManager.handleInput(inputManager, camera, dt);
    handlePlayerInput(dt);
}

void Game::handlePlayerInput(float dt)
{
    const auto       &actionBind        = inputManager.getActionBinding();
    static const auto horizonalWalkAxis = actionBind.getActionName("MoveX");
    static const auto verticalWalkAxis  = actionBind.getActionName("MoveY");

    const auto moveStickState =
        glm::vec2(actionBind.getAxisvalue(horizonalWalkAxis), actionBind.getAxisvalue(verticalWalkAxis));

    glm::vec3 moveDir{};

    if (moveStickState != glm::vec2{})
    {

        auto cameraFrontProj = camera.getTransform().front();
        cameraFrontProj.y    = 0.f;
        cameraFrontProj      = glm::normalize(cameraFrontProj);

        const auto cameraRightProj = glm::normalize(glm::cross(cameraFrontProj, qConstant::AXES_UP));

        const auto rightMovement = cameraRightProj * moveStickState.x;
        const auto upMovement    = cameraFrontProj * (-moveStickState.y);
        moveDir                  = glm::normalize(rightMovement + upMovement);
        const auto angle         = std::atan2(moveDir.x, moveDir.z);
        const auto targetHeading = glm::angleAxis(angle, qConstant::AXES_UP);
        EnttUtil::smoothRotate(playerHandle, targetHeading, dt);
    }

    auto &tCom     = playerHandle.get<TransformComponent>();
    auto &movement = playerHandle.get<MovementComponent>();

    movement.velocity = moveDir;
    auto pos          = tCom.transform.getPosition();

    auto newPos = pos + moveDir * 10.f * dt;

    // update new transforn matrix
    tCom.transform.setPosition(newPos);
    tCom.worldTransform = tCom.transform.getTransformMatrix();
}
void Game::updateLogic(float dt)
{
    MovementSystem::update(registry, dt);
    cameraManager.update(camera, dt);

    // TransformSystem::update(registry, dt);
    if (playerHandle.entity() != entt::null)
    {
        PlayerSkeletonSystem::update(playerHandle, dt);
    }
    SkeletonSystem::update(registry, dt);
}