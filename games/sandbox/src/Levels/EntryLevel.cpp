#include "EntryLevel.h"
#include "GameUtil.h"

#include "ECS/System/System.h"
#include "Level/GameScenePool.h"
#include "MainLevel.h"

void EntryScene::init(GPUDevice &device, std::string name, glm::ivec2 renderSize)
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

    // inputManager->loadInputBinding("assets/settings/input_actions.json", "assets/settings/input_mapping.json");
    windowSize = renderSize;
    renderer->init(device, {renderSize.x, renderSize.y});
    std::filesystem::path levelPath("assets/levels/castle.json");
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
    auto sound = SoundPool::GetInstance()->getSound("step1");
    assert(sound);
    sound->play();
}

void EntryScene::update(float dt)
{
    handleInput(dt);
    updateGameLogic(dt);
}

void EntryScene::draw()
{
    { // collect data
        initDrawObjects();
    }
    // render
    auto sceneData = GameRenderer::SceneProps{
        .camera           = camera,
        .ambientColor     = level.ambientColor,
        .ambientIntensity = level.ambientIntensity,
    };

    auto cmd = device->beginFrame();
    renderer->draw(*device, cmd, camera, sceneData);
    auto &drawImage = renderer->getDrawImage(*device);
    // end render

    GPUDevice::FrameProperties props = {
        .clearColor           = qColor::LinearColor(0, 0, 0, 1.f),
        .copyImageToSwapchain = true,
    };
    device->endFrame(cmd, drawImage, props);
}

void EntryScene::handleInput(float dt)
{
    { // handle camera input
        cameraManager.handleInput(*inputManager, camera, dt);
    }
    { // handle player input
        const auto       &actionBind        = inputManager->getActionBinding();
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
    {
        if (inputManager->getKeyboard().isPressed(GLFW_KEY_ESCAPE))
        {
            auto mainScene = std::make_shared<MainScene>();
            mainScene->init(*device, "mainScene", windowSize);
            GameScenePool::GetInstance()->pushGameScene("mainScene", mainScene);
            GameScenePool::GetInstance()->changeGameScene("mainScene");
        }
    }
}

void EntryScene::updateGameLogic(float dt)
{
    MovementSystem::update(registry, dt);
    cameraManager.update(camera, dt);

    TransformSystem::update(registry, dt);
    if (playerHandle.entity() != entt::null)
    {
        PlayerSkeletonSystem::update(playerHandle, dt);
    }
    SkeletonSystem::update(registry, dt);
}

void EntryScene::initDrawObjects()
{
    renderer->collectDataBegin(*device);
    // colect light data
    const auto light = registry.view<LightComponent, TransformComponent>();
    for (auto &&[entity, light, transform] : light.each())
    {
        renderer->addLight(light.light, transform.transform);
    }
    // render static meshes
    auto meshes = registry.view<TransformComponent, MeshComponent>(entt::exclude<SkeletonComponent>);
    for (auto &&[entity, transform, mesh] : meshes.each())
    {
        for (std::size_t i = 0; i < mesh.meshes.size(); ++i)
        {
            auto transformMat = mesh.transform[i].isIdentity()
                                    ? transform.worldTransform
                                    : transform.worldTransform * mesh.transform[i].getTransformMatrix();
            auto meshObj      = meshPool->getMesh(mesh.meshes[i]);
            renderer->addMesh(mesh.meshes[i], mesh.materials[i], transformMat, mesh.castShadow);
        }
    }

    auto skinmeshes = registry.view<TransformComponent, MeshComponent, SkeletonComponent>();
    for (auto &&[entity, cTransform, cMesh, cSke] : skinmeshes.each())
    {
        auto jointStartIndex = renderer->addJointMatricies(*device, cSke.animator.jointMatricies);
        for (auto i = 0; i < cMesh.meshes.size(); i++)
        {
            renderer->addSkeletonMesh(
                cMesh.meshes[i], cMesh.materials[i], cTransform.worldTransform, cSke.skinMeshes[i], jointStartIndex);
        }
    }
    renderer->collectDataEnd();
}

void EntryScene::initEntityLoader()
{
    enttLoader->setDefaultInitCallback([](entt::registry &registry) {
        auto entity = registry.create();
        registry.emplace<TransformComponent>(entity);
        registry.emplace<HierarchyComponent>(entity);
        registry.emplace<SceneComponent>(entity);
        return entt::handle(registry, entity);
    });

    auto prefabsDir = std::filesystem::path("assets/prefabs");
    GameUtil::forEachFileInDir(prefabsDir, [this, &prefabsDir](const std::filesystem::path &path) {
        auto       relPath  = path.lexically_relative(prefabsDir);
        const auto fileName = relPath.replace_extension("").string();
        enttLoader->registerPrefab(fileName, path);
    });
}

void EntryScene::registerComponent(ComponentLoader &loader)
{
    loader.registerComponent<CameraComponent>("camera");
    loader.registerComponent<SpawnComponent>("player_spawn");
    loader.registerComponent<PlayerComponent>("player");

    loader.registerComponent("movement", [](entt::handle e, MovementComponent &mc, const JsonNode &loader) {
        loader.get("maxSpeed", mc.maxSpeed);
    });

    loader.registerComponent("scene", [](entt::handle handle, SceneComponent &comp, const JsonNode &node) {
        node.getIfExists("scene", comp.sceneName);
        node.getIfExists("node", comp.sceneNodeName);
    });
    loader.registerComponent("mesh", [](entt::handle handle, MeshComponent &comp, const JsonNode &node) {
        node.getIfExists("castShadow", comp.castShadow);
    });
    loader.registerComponent("animation_sound",
                             [](entt::handle handle, AnimationSoundComponent &sc, const JsonNode &node) {
                                 sc.eventSounds = node.getNode("events").getKeyValueMapString();
                                 for (auto &[name, soundPath] : sc.eventSounds)
                                 {
                                     SoundPool::GetInstance()->loadSound(name, soundPath);
                                 }
                             });
}

void EntryScene::postInitEnttCallback(entt::handle handle)
{
    entt::entity entt              = handle.entity();
    auto         skeletonComponent = handle.try_get<SkeletonComponent>();
    if (skeletonComponent)
    {
        initAnimationEntity(handle);
    }

    if (handle.any_of<CameraComponent, PlayerComponent, SpawnComponent>())
    {
        auto &name = handle.get<SceneComponent>().sceneNodeName;
        if (name.empty())
        {
            return; // do not registering any for this component
        }
        handle.get_or_emplace<NameComponent>().name = GameUtil::capComponentNodeName(name);
    }
    for (const auto &c : handle.get<HierarchyComponent>().children)
    {
        postInitEnttCallback(c);
    }
}

void EntryScene::loadLevel(std::filesystem::path &path)
{
    if (std::filesystem::exists(path))
    {
        level.loadLevelFile(path);
    }

    level.levelName = path.filename().replace_extension("").string();

    if (!level.cubemapPath.empty())
    {
        const auto imgNames = std::array{
            std::string("right.jpg"),
            std::string("left.jpg"),
            std::string("top.jpg"),
            std::string("bottom.jpg"),
            std::string("front.jpg"),
            std::string("back.jpg"),
        };
        const auto cubemapImageId = imagePool->loadCubemap(level.cubemapPath, imgNames);
        renderer->setCubemapImage(cubemapImageId);
    }
    else
    {
        // no skybox
        renderer->setCubemapImage(qTypes::NULL_IMAGE_ID);
    }

    const auto &scene           = scenePool->loadScene(level.scenePath.string());
    auto        createdEntities = enttLoader->createEntitiesFromScene(scene);

    TransformSystem::update(registry, 0.f);
}

void EntryScene::initAnimationEntity(entt::handle handle)
{
    auto &skeletonComponent = handle.get<SkeletonComponent>();
    assert(skeletonComponent.skinId != -1);

    const auto &sceneComponent = handle.get<SceneComponent>();
    const auto &scene          = scenePool->loadScene(sceneComponent.sceneName);

    skeletonComponent.skeleton   = scene.skeletons[skeletonComponent.skinId];
    skeletonComponent.animations = &animationPool->getAnimations(sceneComponent.sceneName);

    auto &meshComponent = handle.get<MeshComponent>();
    skeletonComponent.skinMeshes.reserve(meshComponent.meshes.size());
    for (auto meshId : meshComponent.meshes)
    {
        SkinningMesh skinMesh;
        auto        &mesh = meshPool->getMesh(meshId);
        skinMesh.skinnedVertexBuffer =
            GPUBufferBuilder{*device, mesh.numVertices * sizeof(MeshProps)}
                .setBufferUsageFlags(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
                .build();
        skeletonComponent.skinMeshes.push_back(skinMesh);
    }
    if (skeletonComponent.animations->contains("Run"))
    {
        skeletonComponent.animator.assignAnimation(skeletonComponent.skeleton, skeletonComponent.animations->at("Run"));
    }
}
