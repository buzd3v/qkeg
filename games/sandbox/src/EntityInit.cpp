#include "ECS/Component/Component.h"
#include "Game.h"

void forEach(const std::filesystem::path &dir, std::function<void(const std::filesystem::path &)> func)
{
    for (const auto &file : std::filesystem::recursive_directory_iterator(dir))
    {
        if (std::filesystem::is_regular_file(file))
        {
            func(file.path());
        }
    }
}

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
    forEach(prefabsDir, [this, &prefabsDir](const std::filesystem::path &path) {
        auto       relPath  = path.lexically_relative(prefabsDir);
        const auto fileName = relPath.replace_extension("").string();
        enttLoader->registerPrefab(fileName, path);
    });
}

void Game::registerComponent(ComponentLoader &loader)
{
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
    for (const auto &c : handle.get<HierarchyComponent>().children)
    {
        postInitEnttCallback(c);
    }
}
