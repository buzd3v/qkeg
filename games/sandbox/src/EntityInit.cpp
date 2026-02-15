#include "ECS/Component/Component.h"
#include "Game.h"
#include "GameUtil.h"
void Game::initEntityLoader()
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

void Game::registerComponent(ComponentLoader &loader)
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
}

void Game::postInitEnttCallback(entt::handle handle)
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

void Game::initAnimationEntity(entt::handle handle)
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
            GPUBufferBuilder{gpuDevice, mesh.numVertices * sizeof(MeshProps)}
                .setBufferUsageFlags(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
                .build();
        skeletonComponent.skinMeshes.push_back(skinMesh);
    }
    if (skeletonComponent.animations->contains("Idle"))
    {
        skeletonComponent.animator.assignAnimation(skeletonComponent.skeleton,
                                                   skeletonComponent.animations->at("Idle"));
    }
}