#include "DynamicCameraHandler.h"

void DynamicCameraHandler::update(Camera &camera, float dt)
{
    auto newPos = camera.getPosititon();
    newPos += moveVec * dt;
    camera.setPosition(newPos);

    const auto deltaYaw   = rotvec.x * dt;
    const auto deltaPitch = rotvec.y * dt;
    const auto dYaw       = glm::angleAxis(deltaYaw, qConstant::AXES_UP);
    const auto dPitch     = glm::angleAxis(deltaPitch, qConstant::AXES_RIGHT);
    camera.setRotation(dYaw * camera.getRotation() * dPitch);
}

void DynamicCameraHandler::processInput(const InputManager &iManager, Camera &camera, float dt)
{
    auto       &actionBinding = iManager.getActionBinding();
    const auto &moveX         = actionBinding.getActionName("CameraMoveX");
    const auto &moveY         = actionBinding.getActionName("CameraMoveY");
    const auto &moveZ         = actionBinding.getActionName("CameraMoveZ");
    const auto &moveFast      = actionBinding.getActionName("CameraMoveFast");

    auto front = camera.getTransform().front();
    auto right = camera.getTransform().right();

    // glm::vec2  stick(15, 15);
    glm::vec2  stick  = glm::vec2{actionBinding.getAxisvalue(moveX), actionBinding.getAxisvalue(moveY)};
    const auto zStick = actionBinding.getAxisvalue(moveZ);

    glm::vec3 moveDir{};
    moveDir += front * (-stick.y);
    moveDir += right * (stick.x);
    moveDir += qConstant::AXES_UP * zStick;

    moveVec = moveDir * speed;
    if (actionBinding.isHeld(moveFast))
    {
        moveVec *= 2.f;
    }

    const auto rotateXAxis = actionBinding.getActionName("CameraRotateX");
    const auto rotateYAxis = actionBinding.getActionName("CameraRotateY");
    const auto rotateStickState =
        glm::vec2{actionBinding.getAxisvalue(rotateXAxis), actionBinding.getAxisvalue(rotateYAxis)};

    rotvec.x = -rotateStickState.x * yawRotSpeed;
    rotvec.y = -rotateStickState.y * pitchRotSpeed;
}
