#pragma once
#include "ComponentLoader.h"
#include "Renderer/Scene.h"
#include <JsonParser/JsonFile.h>
#include <entt/entt.hpp>
#include <filesystem>
class ScenePool;
class EntityLoader
{
  public:
    using DefaultInitEnttCallbacks = std::function<entt::handle(entt::registry &)>;
    using PostInitEnttCallbacks    = std::function<void(entt::handle)>;

  public:
    EntityLoader(entt::registry &reg, std::string defaultPrefabName);

    entt::handle              createEntityFromPrefab(std::string prefabName, bool callPostInit = true);
    std::vector<entt::handle> createEntitiesFromScene(const Scene &scene);

  private:
    entt::handle createEntityFromNode(const std::string &prefabName, const Scene &scene, const SceneNode &sceneNode);
    void         loadNode(entt::handle handle, const Scene &scene, const SceneNode &node);
    ScenePool   *scenePool;

  public:
    void setDefaultInitCallback(DefaultInitEnttCallbacks callback);
    void setPostInitEnttCallback(PostInitEnttCallbacks callback);
    void registerPrefab(std::string prefabName, const std::filesystem::path &path);
    void addPrefabFile(std::string prefabName, JsonFile file);
    void addMappedPrefabName(const std::string &from, const std::string &to);
    bool isPrefabExist(const std::string &prefabName) const;

    entt::handle       createDefaultEntity(bool postInit = true);
    entt::handle       createEntity(std::string prefabName, const nlohmann::json &overrideData = {});
    const std::string &getMappedPrefabName(const std::string &prefabName) const;

    ComponentLoader &getComponentLoader() { return componentLoader; }

  private:
    JsonNode     getPrefabNode(const std::string &prefabName) const;
    entt::handle createEntity(const std::string &prefabName, const JsonNode &node);

    entt::registry                                  &registry;
    std::function<entt::handle(entt::registry &reg)> defaultInitEnttCallback;
    std::function<void(entt::handle handle)>         defaultPostInitEnttCallback;
    std::unordered_map<std::string, JsonFile>        loadedPrefabsFiles;
    std::unordered_map<std::string, std::string>     prefabNameMapping;
    std::string                                      defaultPrefabName;

    ComponentLoader componentLoader;
};
