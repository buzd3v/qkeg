#include "CameraManager.h"

CameraManager::CameraManager()
{
    addHandler("static", std::make_unique<CameraHandler>());
    setHandler("static");
}

void CameraManager::setHandler(std::string name)
{
    currentHandler = handlers.at(name).get();
    currentName    = name;
}

void CameraManager::handleInput(const InputManager &iManager, Camera &camera, float dt)
{
    assert(currentHandler);
    currentHandler->processInput(iManager, camera, dt);
}

void CameraManager::update(Camera &cam, float dt)
{
    currentHandler->update(cam, dt);
}

void CameraManager::addHandler(std::string name, std::unique_ptr<CameraHandler> handler)
{
    auto [as, inserted] = handlers.emplace(name, std::move(handler));
    assert(inserted);
}

void CameraManager::setCamera(const Transform &transform, Camera &cam, float tranTime)
{
    cam.setPosition(transform.getPosition());
    cam.setRotation(transform.getRotation());
}

CameraHandler &CameraManager::getHandler(std::string name)
{
    return *handlers.at(name);
}
