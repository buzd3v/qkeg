#pragma once

#include "Camera.h"
#include "Input/InputManager.h"
#include "Math/Transform.h"
class CameraHandler
{
  public:
    virtual ~CameraHandler() = default;
    virtual void      processInput(const InputManager &iManager, Camera &camera, float dt) {}
    virtual void      update(Camera &camera, float dt) {}
    virtual Transform getTranform() { return {}; }
};