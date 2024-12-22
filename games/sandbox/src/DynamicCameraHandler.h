#pragma once

#include "Camera/CameraHandler.h"

class DynamicCameraHandler : public CameraHandler
{
  public:
    virtual void processInput(const InputManager &iManager, Camera &camera, float dt);
    virtual void update(Camera &camera, float dt);

    float yawRotSpeed{1.f};
    float pitchRotSpeed{1.f};

  private:
    float dynamicYawAngle{0.f};
    float dynamicPitchAngle{0.f};

    glm::vec3 moveVec{};
    glm::vec3 speed{10.f, 5.f, 10.f};
    glm::vec2 rotvec{};
};