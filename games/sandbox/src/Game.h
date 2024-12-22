#pragma once
#include "Application.h"

#include <entt/entt.hpp>

#include "Graphics/GPUDevice.h"
#include "Graphics/Pipeline/GradientPipeline.h"
#include "Graphics/Renderer/GameRenderer.h"
#include "Graphics/Vulkan/GPUImage.h"
#include "Level/Level.h"
#include "Renderer/AnimationPool.h"
#include "Renderer/ScenePool.h"

#include "Camera/Camera.h"
#include "CameraManager.h"
#include "DynamicCameraHandler.h"
#include "TrackingCameraHandler.h"


#include "ECS/ComponentLoader.h"
#include "ECS/EntityLoader.h"
#include "ECS/EntityUtil.h"

class Game : public Application
{
  public:
    Game();

    virtual void loadAppSetting() override;

    virtual void customInit() override;
    virtual void customUpdate(float dt) override;
    virtual void customDraw() override;
    virtual void customCleanup() override;

    void initDrawObjects();

    // entity management;
    void initEntityLoader();
    void registerComponent(ComponentLoader &comp);
    void postInitEnttCallback(entt::handle handle);
    void loadLevel(std::filesystem::path &path);
    void initAnimationEntity(entt::handle handle);

    void handleInput(float deltaTime);
    void handlePlayerInput(float dt);

  private:
    ImageId          gameImage{qTypes::NULL_IMAGE_ID};
    GradientPipeline gradPipeline;
    GameRenderer    *renderer;
    MeshPool        *meshPool;
    MaterialPool    *materialPool;
    ImagePool       *imagePool;
    ScenePool       *scenePool;
    AnimationPool   *animationPool;

    CameraManager cameraManager;
    std::string   dynamicCameraTag{"dynamic"};
    std::string   trackingCameraTag{"tracking"};

    Camera        camera;

    entt::registry registry;
    EntityLoader  *enttLoader;

    Level level;
};
