#include "Game.h"

#include <utility>

#include "Cubemap.h"
#include "ECS/Component/Component.h"
#include "ECS/System/TransformSystem.h"
#include "JsonParser/JsonFile.h"
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
    imagePool    = ImagePool::GetInstance();
    meshPool     = MeshPool::GetInstance();
    materialPool = MaterialPool::GetInstance();
    scenePool    = ScenePool::GetInstance();

    enttLoader = new EntityLoader(registry, "static_geometry");
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
    std::filesystem::path levelPath("assets/levels/castle.json");
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
    gradPipeline.init(gpuDevice);
    // camera.setPosition(glm::vec3(31.7f, 0.6f, -12.3f));
    // camera.setRotation(glm::quat(0.1099f, 0.57f, 0.00771f, -0.8090f));
    // camera.setUseReverseYaxis(true);
    // camera.setUseInverseDepth(true);
}

void Game::customUpdate(float dt) {}

void Game::customDraw()
{
    { // scope init draw objs
        initDrawObjects();
    }
    // auto cmd       = gpuDevice.beginFrame();
    // auto drawImage = gpuDevice.queryImage(gameImage);
    // VkUtil::transitionImage(cmd, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED,
    // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL); gradPipeline.draw(cmd, gpuDevice, drawImage);

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