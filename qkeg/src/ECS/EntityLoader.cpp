#include "ECS/EntityLoader.h"
#include "ECS/EntityUtil.h"

#include "ECS/Component/Component.h"
#include "Renderer/ScenePool.h"

nlohmann::json mergeJson(const nlohmann::json &a, const nlohmann::json &b)
{
    auto       result = a.flatten();
    const auto tmp    = b.flatten();
    for (const auto &[k, v] : tmp.items())
    {
        result[k] = v;
    }
    return result.unflatten();
}

void EntityLoader::setDefaultInitCallback(DefaultInitEnttCallbacks callback)
{
    defaultInitEnttCallback = callback;
}

void EntityLoader::setPostInitEnttCallback(PostInitEnttCallbacks callback)
{
    defaultPostInitEnttCallback = callback;
}

void EntityLoader::registerPrefab(std::string prefabName, const std::filesystem::path &path)
{
    JsonFile file(path);
    assert(file.isGood());

    addPrefabFile(std::move(prefabName), std::move(file));
}

void EntityLoader::addPrefabFile(std::string prefabName, JsonFile file)
{
    assert(file.isGood());
    auto [it, inserted] = loadedPrefabsFiles.emplace(prefabName, std::move(file));
    if (!inserted)
        return;
}

bool EntityLoader::isPrefabExist(const std::string &prefabName) const
{
    return loadedPrefabsFiles.contains(prefabName);
}

void EntityLoader::addMappedPrefabName(const std::string &from, const std::string &to)
{
    if (from == to)
        return;
    if (prefabNameMapping.contains(from))
    {
        return;
    }
    if (isPrefabExist(to))
    {
        return;
    }
    auto mapped = getMappedPrefabName(to);
    if (!mapped.empty())
    {
        prefabNameMapping.emplace(from, mapped);
        return;
    }
    fmt::println("Cannot add prefab name: {}", to);
}

entt::handle EntityLoader::createDefaultEntity(bool postInit)
{
    auto handle =
        defaultInitEnttCallback ? defaultInitEnttCallback(registry) : entt::handle{registry, registry.create()};

    if (!handle.all_of<MetadataComponent>())
    {
        handle.emplace<MetadataComponent>();
    }
    if (postInit && defaultPostInitEnttCallback)
    {
        defaultPostInitEnttCallback(handle);
    }
    return handle;
}

entt::handle EntityLoader::createEntity(const std::string &prefabName, const JsonNode &node)
{
    auto handle = createDefaultEntity(false);
    for (const auto &[componentName, componentNode] : node.getKeyValueMap())
    {
        if (!componentLoader.isRegistered(componentName))
        {
            continue;
        }
        componentLoader.makeComponent(handle, componentName, componentNode);
    }
    handle.get<MetadataComponent>().debugString = prefabName;

    if (defaultPostInitEnttCallback)
    {
        defaultPostInitEnttCallback(handle);
    }
    return handle;
}

JsonNode EntityLoader::getPrefabNode(const std::string &prefabName) const
{
    auto pref = loadedPrefabsFiles.find(prefabName);
    if (pref == loadedPrefabsFiles.end())
    {
        throw std::runtime_error("");
    }
    return pref->second.getRootNode();
}
entt::handle EntityLoader::createEntity(std::string prefabName, const nlohmann::json &overrideData)
{
    const auto &mappedPrefabName = getMappedPrefabName(prefabName);
    const auto &actualPrefabName = !mappedPrefabName.empty() ? mappedPrefabName : prefabName;
    const auto  prefabLoader     = getPrefabNode(actualPrefabName);

    // merge override data (if present)
    if (!overrideData.empty())
    {
        const auto &originalData = prefabLoader.getJson();
        const auto  jsonData     = mergeJson(originalData, overrideData);
        JsonNode    loader{jsonData, prefabLoader.getName() + "(+ overload data)"};
        return createEntity(actualPrefabName, loader);
    }

    return createEntity(actualPrefabName, prefabLoader);
}

const std::string &EntityLoader::getMappedPrefabName(const std::string &prefabName) const
{
    // TODO: insert return statement here
    if (const auto it = prefabNameMapping.find(prefabName); it != prefabNameMapping.end())
    {
        return it->second;
    }
    if (isPrefabExist(prefabName))
    {
        return prefabName;
    }

    return "";
}
////////////////////////////////////////////////////////////////////
/*
//
//
//
//
//
//
//
//
*/
////////////////////////////////////////////////////////////////////

EntityLoader::EntityLoader(entt::registry &reg, std::string defaultPrefabName)
    : registry(reg), defaultPrefabName(defaultPrefabName)
{
    scenePool = ScenePool::GetInstance();
}
entt::handle EntityLoader::createEntityFromPrefab(std::string prefabName, bool callPostInit)
{
    // assert(defaultPostInitEnttCallback);
    auto handle = createEntity(prefabName);

    auto &sceneComponent = handle.get<SceneComponent>();
    if (!sceneComponent.sceneName.empty())
    {
        const auto &scene = scenePool->loadScene(sceneComponent.sceneName);
        assert(scene.nodes.size() == 1);
        auto &rootNode = *scene.nodes[0];
        loadNode(handle, scene, rootNode);
        sceneComponent.sceneCompName = sceneComponent.sceneName;
        sceneComponent.sceneNodeName = rootNode.name;
    }
    if (callPostInit)
    {
        defaultPostInitEnttCallback(handle);
    }
    return handle;
}
std::vector<entt::handle> EntityLoader::createEntitiesFromScene(const Scene &scene)
{
    assert(defaultPostInitEnttCallback);
    assert(isPrefabExist("camera"));
    assert(isPrefabExist("light"));

    std::vector<entt::handle> entts;
    for (const auto &rootNode : scene.nodes)
    {
        auto prefabName = EnttUtil::getPrefabNameFromSceneNode(*this, *rootNode, defaultPrefabName);
        auto handle     = createEntityFromNode(prefabName, scene, *rootNode);
        entts.emplace_back(std::move(handle));
    }
    for (auto &e : entts)
    {
        defaultPostInitEnttCallback(e);
    }

    return entts;
}

entt::handle EntityLoader::createEntityFromNode(const std::string &prefabName, const Scene &scene,
                                                const SceneNode &sceneNode)
{
    auto handle = createEntityFromPrefab(prefabName, false);

    auto &sceneComponent = handle.get<SceneComponent>();
    if (!sceneComponent.sceneName.empty() && sceneNode.children.size() == 1 && sceneNode.children[0]->meshIndex != -1)
    {
        handle.get<TransformComponent>().transform = sceneNode.transform;
    }
    else
    {
        loadNode(handle, scene, sceneNode);
        sceneComponent.sceneCompName = scene.path.string();
        sceneComponent.sceneNodeName = sceneNode.name;
    }
    return handle;
}

void appendMesh(MeshComponent &mc, const ModelPool &model, const glm::mat4 &transform)
{
    for (std::size_t i = 0; i < model.primitiveMeshes.size(); ++i)
    {
        mc.meshes.push_back(model.primitiveMeshes[i]);
        mc.materials.push_back(model.primitiveMaterials[i]);
        mc.transform.push_back(transform);
    }
}

void EntityLoader::loadNode(entt::handle handle, const Scene &scene, const SceneNode &node)
{
    auto &cTransform = handle.get<TransformComponent>();

    const auto prevTransform = cTransform.transform;
    cTransform.transform     = node.transform;

    if (!prevTransform.isIdentity())
    {
        cTransform.transform =
            Transform(cTransform.transform.getTransformMatrix() * prevTransform.getTransformMatrix());
    }

    if (node.skeletonIndex != -1)
    {
        auto &cSke  = handle.get_or_emplace<SkeletonComponent>();
        cSke.skinId = node.skeletonIndex;
    }

    if (node.cameraIndex != -1)
    {
        assert(handle.get<MetadataComponent>().debugString == "camera");

        auto &cTransform = handle.get_or_emplace<TransformComponent>();
        cTransform.transform.setRotation(glm::angleAxis(glm::radians(180.f), cTransform.transform.up()) *
                                         cTransform.transform.getRotation());
    }

    if (node.lightIndex != -1)
    {
        assert(handle.get<MetadataComponent>().debugString == "light");
        auto &clight = handle.get_or_emplace<LightComponent>();
        clight.light = scene.lights[node.lightIndex];
    }

    // copy meshes
    if (node.meshIndex != -1)
    {
        auto &cMesh = handle.get_or_emplace<MeshComponent>();
        if (cMesh.meshes.empty())
        {
            const auto &sceneModel = scene.models[node.meshIndex];
            appendMesh(cMesh, sceneModel, qConstant::IDENTITY_MAT);
        }
        else
        {
        }
        auto c = 10;
    }

    // handle children
    for (const auto &pChildNode : node.children)
    {
        auto       &childNode           = *pChildNode;
        const auto &childNodePrefabName = EnttUtil::getPrefabNameFromSceneNode(*this, childNode, defaultPrefabName);

        if (childNodePrefabName != defaultPrefabName)
        {
            auto child = createEntityFromNode(childNodePrefabName, scene, childNode);
            EnttUtil::addChild(handle, child);
            continue;
        }

        assert(!childNode.children.empty());

        if (childNode.meshIndex != -1)
        {
            // merge child meshes into the parent node
            auto       &mc         = handle.get_or_emplace<MeshComponent>();
            const auto &childModel = scene.models[childNode.meshIndex];
            appendMesh(mc, childModel, childNode.transform.getTransformMatrix());
        }
    }
}