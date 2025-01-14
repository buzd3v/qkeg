#include "EntryLevel.h"
void EntryScene::init(GPUDevice &device, std::string name, glm::vec2 renderSize)
{
    GameScene::init(device, name, renderSize);

    enttLoader = new EntityLoader(registry, "static_geometry");
    std::filesystem::path aniPath("assets/settings/animation.json");
    animationPool->loadFromDirectory(aniPath);
    // init enttity
    initEntityLoader();
    registerComponent(enttLoader->getComponentLoader());
    enttLoader->setPostInitEnttCallback([this](entt::handle handle) { postInitEnttCallback(handle); });

    auto createInfo = GPUImageCreateInfo{
        .format     = VK_FORMAT_R16G16B16A16_SFLOAT,
        .usageFlags = qConstant::defaultDrawImageUsage,
        .extent =
            {
                .width  = (uint32_t)renderSize.x,
                .height = (uint32_t)renderSize.y,
                .depth  = 1,
            },
    };
    gameImage = imagePool->createImage(createInfo);

    inputManager->loadInputBinding("assets/settings/input_actions.json", "assets/settings/input_mapping.json");

    renderer->init(device, {renderSize.x, renderSize.y});
    std::filesystem::path levelPath("assets/levels/city.json");
    loadLevel(levelPath);

    { // create camera
        static const float aspectRatio = (float)renderSize.x / (float)renderSize.y;

        camera.setUseInverseDepth(true);
        camera.setPosition(glm::vec3(0, 0, -5));
        camera.initCamera(Camera::ProjectionType::Perspective,
                          qConstant::DEFAULT_X_FOV,
                          qConstant::DEFAULT_NEAR_PLANE,
                          qConstant::DEFAULT_FAR_PLANE,
                          aspectRatio);
    }

    playerHandle = enttLoader->createEntityFromPrefab("cato");
    { // init player
        playerHandle.emplace<PlayerComponent>();
        auto &skeletonComp = playerHandle.get<SkeletonComponent>();
        auto &sc           = skeletonComp;
        assert(sc.animations);
        sc.animator.assignAnimation(sc.skeleton, sc.animations->at("Idle"));
        auto playerSpawnHandle = EnttUtil::findEnttByName<SpawnComponent>(registry, level.playerSpawn);
        if (playerSpawnHandle.entity() == entt::null)
        {
            fmt::println("could not find {}", level.playerSpawn);
            return;
        }
        auto &spawnComponent     = playerSpawnHandle.get<TransformComponent>();
        auto  spawnPos           = spawnComponent.transform.getPosition();
        auto &transformComponent = playerHandle.get<TransformComponent>();
        // auto  up                 = qConstant::AXES_UP;
        transformComponent.transform.setRotation(spawnComponent.transform.getRotation());
        transformComponent.transform.setPosition(spawnPos);
        transformComponent.worldTransform = transformComponent.transform.getTransformMatrix();
    }

    { // camera handler
        auto trackCam = std::make_unique<TrackingCameraHandler>();
        trackCam->initialize(camera);
        cameraManager.addHandler(dynamicCameraTag, std::make_unique<DynamicCameraHandler>());
        cameraManager.addHandler(trackingCameraTag, std::move(trackCam));

        auto camEntt       = EnttUtil::findEnttByName<CameraComponent>(registry, level.cameraSpawn);
        bool shouldProcess = true;
        if (camEntt.entity() == entt::null)
        {
            fmt::println("could not find {}", level.cameraSpawn);
            shouldProcess = false;
        }
        if (shouldProcess)
        {
            const auto &transformComponent = camEntt.get<TransformComponent>();
            cameraManager.setCamera(transformComponent.transform, camera);
            cameraManager.setHandler(dynamicCameraTag);
        }
        else
        {
            cameraManager.setCamera(qConstant::IDENTITY_MAT, camera);
            cameraManager.setHandler(trackingCameraTag);
            auto &tch = static_cast<TrackingCameraHandler &>(cameraManager.getHandler(trackingCameraTag));
            tch.trackingEntity(playerHandle, camera);
        }
        // cameraManager.setHandler(dynamicCameraTag);
    }
}

void EntryScene::update(float dt) {}

void EntryScene::draw() {}

void EntryScene::handleInput() {}

void EntryScene::updateGameLogic() {}

void EntryScene::initDrawObjects() {}

void EntryScene::initEntityLoader() {}

void EntryScene::registerComponent(ComponentLoader &comp) {}

void EntryScene::postInitEnttCallback(entt::handle handle) {}

void EntryScene::loadLevel(std::filesystem::path &path) {}

void EntryScene::initAnimationEntity(entt::handle handle) {}
