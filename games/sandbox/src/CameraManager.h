#pragma once

#include "Camera/CameraHandler.h"

class CameraManager
{
  public:
    CameraManager();
    void handleInput(const InputManager &iManager, Camera &camera, float dt);
    void update(Camera &cam, float dt);

    void addHandler(std::string name, std::unique_ptr<CameraHandler> handler);
    void setHandler(std::string name);
    void setCamera(const Transform &transform, Camera &cam, float tranTime = 0.f);

    CameraHandler &getHandler(std::string name);

  private:
    CameraHandler *currentHandler{nullptr};
    std::string    currentName{};

    std::unordered_map<std::string, std::unique_ptr<CameraHandler>> handlers;
};