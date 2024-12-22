#include "TrackingCameraHandler.h"
#include "ECS/Component/Component.h"
void TrackingCameraHandler::initialize(Camera &camera)
{
    camera.setRotation(defaultYaw, defaultPitch);
    currentPitch = defaultPitch;
    currentYaw   = defaultYaw;
}

void TrackingCameraHandler::trackingEntity(entt::handle handle, Camera &camera)
{
    objectHandle = handle;
    setWantedTrackingPosition(camera);
}

Transform TrackingCameraHandler::getTranform()
{
    return transform;
}

void TrackingCameraHandler::update(Camera &camera, float dt)
{
    if (objectHandle.entity() == entt::null)
    {
        return;
    }

    if (!objectHandle.valid())
    {
        objectHandle = {};
        return;
    }
    camera.setRotation(defaultYaw, defaultPitch);
    auto &transformComp = objectHandle.get<TransformComponent>();
    setWantedTrackingPosition(camera);
}

void TrackingCameraHandler::processInput(const InputManager &iManager, Camera &camera, float dt)
{
    const auto &actionBind       = iManager.getActionBinding();
    const auto  rotateX          = actionBind.getActionName("CameraRotateX");
    const auto  rotateY          = actionBind.getActionName("CameraRotateY");
    const auto  rotateStickState = glm::vec2{actionBind.getAxisvalue(rotateX), actionBind.getAxisvalue(rotateY)};

    rotvec.x = -rotateStickState.x * yawRotSpeed;
    rotvec.y = -rotateStickState.y * pitchRotSpeed;
}
void TrackingCameraHandler::setWantedTrackingPosition(Camera &camera)
{
    if (objectHandle.entity() == entt::null)
    {
        return;
    }
    float cameraZOffset{0.25f};
    float cameraYOffset{1.5f};
    float orbitDistance{8.5f};
    auto  targetTransform = objectHandle.get<TransformComponent>().transform;

    auto orbitTarget =
        targetTransform.getPosition() + targetTransform.up() * cameraYOffset + targetTransform.front() * cameraZOffset;

    auto target = orbitTarget - camera.getTransform().front() * orbitDistance;
    camera.setPosition(target);

    cameraTrackPoint = orbitTarget;
}