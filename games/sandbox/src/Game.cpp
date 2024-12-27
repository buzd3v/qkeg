#include "Game.h"
#include <utility>

#include "Cubemap.h"
#include "ECS/Component/Component.h"
#include "JsonParser/JsonFile.h"

#include "ECS/System/System.h"
Game::Game() : Application() {}

void Game::loadAppSetting()
{
    const std::filesystem::path settingsPath{"assets/settings/app_settings.json"};
    JsonFile                    file(settingsPath);

    if (!file.isGood())
    {
        throw(std::runtime_error(fmt::format("Cannot open file: {}", settingsPath.string())));
        return;
    }
    const JsonNode   rootNode = file.getRootNode();
    std::vector<int> a;
    params.renderSize = {1920, 1080};
    rootNode.getIfExists("renderResolution", params.renderSize);
    rootNode.getIfExists("appName", params.appName);
}

void Game::customInit()
{
    imagePool     = ImagePool::GetInstance();
    meshPool      = MeshPool::GetInstance();
    materialPool  = MaterialPool::GetInstance();
    scenePool     = ScenePool::GetInstance();
    animationPool = AnimationPool::GetInstance();

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
                .width  = (uint32_t)params.renderSize.x,
                .height = (uint32_t)params.renderSize.y,
                .depth  = 1,
            },
    };
    gameImage = imagePool->createImage(createInfo);

    inputManager.loadInputBinding("assets/settings/input_actions.json", "assets/settings/input_mapping.json");

    GameRenderer::Construct();
    renderer = GameRenderer::GetInstance();
    renderer->init(gpuDevice, params.renderSize);
    std::filesystem::path levelPath("assets/levels/burger_joint.json");
    loadLevel(levelPath);

    { // create camera
        static const float aspectRatio = (float)params.renderSize.x / (float)params.renderSize.y;

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
        sc.animator.assignAnimation(sc.skeleton, sc.animations->at("Walk"));
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

void Game::customUpdate(float dt)
{
    updateLogic(dt);
    handleInput(dt);
}

void Game::customDraw()
{
    { // scope init draw objs
        initDrawObjects();
    }

    // render
    auto sceneData = GameRenderer::SceneProps{
        .camera           = camera,
        .ambientColor     = level.ambientColor,
        .ambientIntensity = level.ambientIntensity,
    };
    auto cmd = gpuDevice.beginFrame();
    renderer->draw(gpuDevice, cmd, camera, sceneData);
    auto &drawImage = renderer->getDrawImage(gpuDevice);
    // end render

    GPUDevice::FrameProperties props = {
        .clearColor           = qColor::LinearColor(0, 0, 0, 1.f),
        .copyImageToSwapchain = true,
    };
    gpuDevice.endFrame(cmd, drawImage, props);
}

void Game::customCleanup()
{
    // gradPipeline.cleanUp(gpuDevice);
    renderer->cleanUp(gpuDevice);
}

void Game::initDrawObjects()
{

    renderer->collectDataBegin(gpuDevice);

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
        auto jointStartIndex = renderer->addJointMatricies(gpuDevice, cSke.animator.jointMatricies);
        for (auto i = 0; i < cMesh.meshes.size(); i++)
        {
            renderer->addSkeletonMesh(
                cMesh.meshes[i], cMesh.materials[i], cTransform.worldTransform, cSke.skinMeshes[i], jointStartIndex);
        }
    }
    renderer->collectDataEnd();
}

void Game::loadLevel(std::filesystem::path &path)
{
    auto imagePooll = ImagePool::GetInstance();
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
        const auto cubemapImageId = imagePooll->loadCubemap(level.cubemapPath, imgNames);
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