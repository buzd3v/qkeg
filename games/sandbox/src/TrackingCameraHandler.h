#pragma once

#include "Camera/CameraHandler.h"
#include "entt/entt.hpp"

class TrackingCameraHandler : public CameraHandler
{
  public:
    virtual void      processInput(const InputManager &iManager, Camera &camera, float dt);
    virtual void      update(Camera &camera, float dt);
    virtual Transform getTranform();

    void initialize(Camera &camera);
    void trackingEntity(entt::handle handle, Camera &camera);
    void setWantedTrackingPosition(Camera &camera);

  private:
    entt::handle objectHandle;
    Transform    transform;
    float        defaultYaw{glm::radians(-135.f)};
    float        defaultPitch{glm::radians(-20.f)};
    float        currentYaw{};
    float        currentPitch{};
    glm::vec2    rotvec{};
    float        yawRotSpeed{1.f};
    float        pitchRotSpeed{1.f};

    glm::vec3 trackCameraV;
    glm::vec3 cameraTrackPoint;
};