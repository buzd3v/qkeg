#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include "Common.h"
#include "GLTFLoader/GLTFLoader.h"
#include "GPUDevice.h"
#include "Renderer/Scene.h"

#include <glm/gtc/type_ptr.hpp>

template <>
struct fastgltf::ElementTraits<std::array<uint8_t, 4>>
    : fastgltf::ElementTraitsBase<std::array<uint8_t, 4>, AccessorType::Vec4, uint8_t>
{
};
template <>
struct fastgltf::ElementTraits<std::array<float, 4>>
    : fastgltf::ElementTraitsBase<std::array<float, 4>, AccessorType::Vec4, float>
{
};

namespace qConstant
{
static const std::string GLTF_POSITIONS_ACCESSOR{"POSITION"};
static const std::string GLTF_NORMALS_ACCESSOR{"NORMAL"};
static const std::string GLTF_TANGENTS_ACCESSOR{"TANGENT"};
static const std::string GLTF_UVS_ACCESSOR{"TEXCOORD_0"};
static const std::string GLTF_JOINTS_ACCESSOR{"JOINTS_0"};
static const std::string GLTF_WEIGHTS_ACCESSOR{"WEIGHTS_0"};

static const std::string GLTF_SAMPLER_PATH_TRANSLATION{"translation"};
static const std::string GLTF_SAMPLER_PATH_ROTATION{"rotation"};
static const std::string GLTF_SAMPLER_PATH_SCALE{"scale"};

// extensions
static const std::string GLTF_LIGHTS_PUNCTUAL_KEY{"KHR_lights_punctual"};
static const std::string GLTF_LIGHTS_PUNCTUAL_POINT_NAME{"point"};
static const std::string GLTF_LIGHTS_PUNCTUAL_DIRECTIONAL_NAME{"directional"};
static const std::string GLTF_LIGHTS_PUNCTUAL_SPOT_NAME{"spot"};

static const std::string GLTF_EMISSIVE_STRENGTH_KHR_KEY{"KHR_materials_emissive_strength"};
static const std::string GLTF_EMISSIVE_STRENGTH_PARAM_NAME{"emissiveStrength"};

} // namespace qConstant

qColor::LinearColor getDiffuseColor(const fastgltf::Material &material)
{
    const auto color = material.pbrData.baseColorFactor;

    assert(color.size() == 4);
    return qColor::LinearColor((float)color[0], (float)color[1], (float)color[2], (float)color[3]);
}

bool hasDiffuseTexture(const fastgltf::Material &material)
{
    const auto hasTex = material.pbrData.baseColorTexture.has_value();
    return hasTex;
}

bool hasNormalMapTexture(const fastgltf::Material &material)
{
    const auto hasTex = material.normalTexture.has_value();
    return hasTex;
}
bool hasMetallicRoughnessTexture(const fastgltf::Material &material)
{
    const auto hasTex = material.pbrData.metallicRoughnessTexture.has_value();
    return hasTex;
}
bool hasEmissiveTexure(const fastgltf::Material &material)
{
    const auto tex = material.emissiveTexture.has_value();
    return tex;
}

std::string getImageGltf(std::string name, std::filesystem::path path)
{
    const auto supported_extension = std::array{".jpg", ".png"};
    for (auto ex : supported_extension)
    {
        auto temp = name + ex;
        if (std::filesystem::exists(path / temp))
            return temp;
    }
    return "";
}

std::string getImageUri(fastgltf::Image img)
{
    std::string path;
    std::visit(fastgltf::visitor{
                   [](auto &arg) {},
                   [&](fastgltf::sources::URI &filePath) {
                       assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
                       assert(filePath.uri.isLocalPath());   // We're only capable of loading local files.
                       path = std::string(filePath.uri.path().begin(), filePath.uri.path().end()); // Thanks C++.
                   },
                   [&](fastgltf::sources::Array &vector) {},
                   [&](fastgltf::sources::BufferView &view) {},
               },
               img.data);
    return path;
}

std::filesystem::path getNormalMapTexturePath(const std::filesystem::path fileDir, const fastgltf::Asset &asset,
                                              const fastgltf::Material &material)
{
    const auto  texIndex = material.normalTexture.value().textureIndex;
    const auto &texId    = asset.textures[texIndex];
    const auto &image    = asset.images[texId.imageIndex.value()];

    return fileDir / getImageGltf(image.name.c_str(), fileDir);
}
std::filesystem::path getEmissiveTexturePath(const std::filesystem::path fileDir, const fastgltf::Asset &asset,
                                             const fastgltf::Material &material)
{
    const auto  texIndex = material.emissiveTexture.value().textureIndex;
    const auto &texId    = asset.textures[texIndex];
    const auto &image    = asset.images[texId.imageIndex.value()];

    return fileDir / getImageGltf(image.name.c_str(), fileDir);
}

std::filesystem::path getDiffuseTexturePath(const std::filesystem::path fileDir, const fastgltf::Asset &asset,
                                            const fastgltf::Material &material)
{
    const auto            texIndex = material.pbrData.baseColorTexture.value().textureIndex;
    const auto           &texId    = asset.textures[texIndex];
    const fastgltf::Image image    = asset.images[texId.imageIndex.value()];

    return fileDir / getImageUri(image);
}

std::filesystem::path getMetallicRoughnessTexture(const std::filesystem::path fileDir, const fastgltf::Asset &asset,
                                                  const fastgltf::Material &material)
{
    const auto  texIndex = material.pbrData.metallicRoughnessTexture.value().textureIndex;
    const auto &texId    = asset.textures[texIndex];
    const auto &image    = asset.images[texId.imageIndex.value()];

    return fileDir / getImageUri(image);
}
float getEmissiveStrength(const fastgltf::Material &material)
{
    return material.emissiveStrength;
}

fastgltf::Asset loadglTF(const std::filesystem::path &path)
{
    fmt::println("Start loading gltf {}", path.string());

    fastgltf::Parser parser{};
    auto             data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.error() != fastgltf::Error::None)
    {
        throw(std::runtime_error(fmt::format("Cannot load file: {}", path.string())));
    }
    return std::move(parser.loadGltf(data.get(), path.parent_path(), fastgltf::Options::None).get());
}

GPUMaterial loadMaterial(GPUDevice &device, const fastgltf::Asset &asset, const std::filesystem::path &fileDir,
                         const fastgltf::Material &glTFMaterial)
{
    GPUMaterial material{
        .baseColor       = getDiffuseColor(glTFMaterial),
        .metalicFactor   = glTFMaterial.pbrData.metallicFactor,
        .roughnessFactor = glTFMaterial.pbrData.roughnessFactor,
        .name            = glTFMaterial.name.c_str(),
    };

    auto imagePool = ImagePool::GetInstance();

    if (hasDiffuseTexture(glTFMaterial))
    {
        auto diffusePath = getDiffuseTexturePath(fileDir, asset, glTFMaterial);
        material.diffuseTexture =
            imagePool->loadImageFromFile(diffusePath, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT, true);
    }
    if (hasEmissiveTexure(glTFMaterial))
    {
        material.emissiveFactor = getEmissiveStrength(glTFMaterial);
        auto emissivePath       = getEmissiveTexturePath(fileDir, asset, glTFMaterial);
        material.emissiveTexture =
            imagePool->loadImageFromFile(emissivePath, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT, true);
    }
    if (hasNormalMapTexture(glTFMaterial))
    {
        auto path = getNormalMapTexturePath(fileDir, asset, glTFMaterial);
        material.normalMapTexture =
            imagePool->loadImageFromFile(path, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, true);
    }
    if (hasMetallicRoughnessTexture(glTFMaterial))
    {
        auto path = getMetallicRoughnessTexture(fileDir, asset, glTFMaterial);
        material.metallicRoughnessTexture =
            imagePool->loadImageFromFile(path, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, true);
    }

    return material;
}

int findAccessor(const fastgltf::Primitive &primitive, const std::string &attrbName)
{
    for (auto &[accessorName, accessorID] : primitive.attributes)
    {
        if (std::string(accessorName.c_str()) == attrbName)
        {
            return (int)accessorID;
        }
    }
    return -1;
}

template <typename Datatype>
std::vector<Datatype> getPackedBuffer(const fastgltf::Asset &asset, const fastgltf::Accessor &accessor)
{
    std::vector<Datatype> resource;

    resource.reserve(accessor.count);
    fastgltf::iterateAccessor<Datatype>(asset, accessor, [&](Datatype var) { resource.push_back(var); });

    return resource;
}

template <typename Datatype>
std::vector<Datatype> getPackedBuffer(const fastgltf::Asset &asset, const fastgltf::Primitive &primitive,
                                      const std::string &name)
{
    auto accesorIndex = findAccessor(primitive, name);
    assert(accesorIndex != -1 && "Not found accessor");
    const auto &accessor = asset.accessors.at(accesorIndex);

    return getPackedBuffer<Datatype>(asset, accessor);
}

MeshProps loadPrimitives(const fastgltf::Asset &asset, const std::string name, const fastgltf::Primitive &primitive)
{
    MeshProps props{.name = name};

    if (primitive.indicesAccessor.has_value())
    {

        auto &indicesAcessor = asset.accessors[primitive.indicesAccessor.value()];
        auto  indices        = getPackedBuffer<uint32_t>(asset, indicesAcessor);
        props.indices.assign(indices.begin(), indices.end());
    }

    { // load positions
        const auto positions = getPackedBuffer<glm::vec3>(asset, primitive, qConstant::GLTF_POSITIONS_ACCESSOR);
        props.vertices.resize(positions.size());
        for (auto i = 0; i < positions.size(); i++)
        {
            props.vertices[i].position = positions[i];
        }
    }

    { // min and max pos
        if (auto posAccessorIndex = findAccessor(primitive, qConstant::GLTF_POSITIONS_ACCESSOR) != -1)
        {
            auto minpos = props.vertices.begin()->position;
            auto maxpos = props.vertices.begin()->position;

            for (auto &pos : props.vertices)
            {
                minpos = glm::min(minpos, pos.position);
                maxpos = glm::max(maxpos, pos.position);
            }

            props.minPos = minpos;
            props.maxPos = maxpos;
        }
    }

    auto numVertices = props.vertices.size();
    { // load normals
        if (auto normalsAcessor = findAccessor(primitive, qConstant::GLTF_NORMALS_ACCESSOR) != -1)
        {
            const auto normals = getPackedBuffer<glm::vec3>(asset, primitive, qConstant::GLTF_NORMALS_ACCESSOR);
            assert(normals.size() == numVertices && "Numbers of normal and vertices are not match");
            for (auto i = 0; i < normals.size(); i++)
            {
                props.vertices[i].normal = normals[i];
            }
        }
    }

    { // load uvs
        if (auto uvsAccessor = findAccessor(primitive, qConstant::GLTF_UVS_ACCESSOR) != -1)
        {
            const auto uvs = getPackedBuffer<glm::vec2>(asset, primitive, qConstant::GLTF_UVS_ACCESSOR);
            assert(uvs.size() == numVertices && "Numbers of uvs and vertices are not match");
            for (auto i = 0; i < uvs.size(); i++)
            {
                props.vertices[i].uv_x = uvs[i].x;
                props.vertices[i].uv_y = uvs[i].y;
            }
        }
    }

    { // load tagents
        if (auto tegentsAcessor = findAccessor(primitive, qConstant::GLTF_TANGENTS_ACCESSOR) != -1)
        {
            const auto tangents = getPackedBuffer<glm::vec4>(asset, primitive, qConstant::GLTF_TANGENTS_ACCESSOR);
            assert(tangents.size() == numVertices && "Numbers of tangents and vertices are not match");
            for (auto i = 0; i < tangents.size(); i++)
            {
                props.vertices[i].tangent = tangents[i];
            }
        }
    }

    { // load joints and weight //for skinning
        if (auto jointsAccessor = findAccessor(primitive, qConstant::GLTF_JOINTS_ACCESSOR) != -1)
        {
            const auto joints =
                getPackedBuffer<std::array<uint8_t, 4>>(asset, primitive, qConstant::GLTF_JOINTS_ACCESSOR);
            const auto weights =
                getPackedBuffer<std::array<float, 4>>(asset, primitive, qConstant::GLTF_WEIGHTS_ACCESSOR);

            assert(joints.size() == numVertices && "Numbers of joints and vertices are not match");
            assert(weights.size() == numVertices && "Numbers of wieghts and vertices are not match");

            props.hasSkeleton = true;
            props.skinningProps.resize(numVertices);

            for (std::size_t i = 0; i < joints.size(); ++i)
            {
                props.skinningProps[i].jointIds = glm::uvec4{joints[i][0], joints[i][1], joints[i][2], joints[i][3]};
            }

            for (std::size_t i = 0; i < weights.size(); ++i)
            {
                props.skinningProps[i].weights = glm::vec4{weights[i][0], weights[i][1], weights[i][2], weights[i][3]};
            }
        }
    }
    return props;
}

LightType chooseLightType(const fastgltf::LightType type)
{
    switch (type)
    {
    case fastgltf::LightType::Point:
        return LightType::Point;
        break;
    case fastgltf::LightType::Directional:
        return LightType::Directional;
        break;
    case fastgltf::LightType::Spot:
        return LightType::Spot;
        break;
    default:
        break;
    }
    return LightType::None;
}

qColor::LinearColor vec2Color(glm::vec3 color)
{
    return qColor::LinearColor{
        .red   = (float)color[0],
        .green = (float)color[1],
        .blue  = (float)color[2],
        .alpha = 1.f,
    };
}

float getIntensity(float inten)
{
    const auto PBR_WATTS_TO_LUMENS = 683.f;
    return inten / PBR_WATTS_TO_LUMENS;
}

Light loadLight(const fastgltf::Light &gltfLight)
{
    Light light{
        .name      = gltfLight.name.c_str(),
        .type      = chooseLightType(gltfLight.type),
        .color     = vec2Color(glm::make_vec3(gltfLight.color.data())),
        .intensity = getIntensity(gltfLight.intensity),

    };
    light.range    = gltfLight.range.has_value() ? (float)gltfLight.range.value() : 0.f;
    auto innerCone = gltfLight.innerConeAngle.has_value() ? gltfLight.innerConeAngle.value() : 0.f;
    auto outerCone = gltfLight.outerConeAngle.has_value() ? gltfLight.outerConeAngle.value() : 0.f;

    light.setConeAngle(innerCone, outerCone);
    return light;
}

Transform gl2Transform(const fastgltf::Node &glNode)
{
    Transform t;
    std::visit(fastgltf::visitor{
                   [&](fastgltf::TRS transform) {
                       glm::vec3 tl(transform.translation[0], transform.translation[1], transform.translation[2]);
                       glm::quat rot(
                           transform.rotation[3], transform.rotation[0], transform.rotation[1], transform.rotation[2]);
                       glm::vec3 sc(transform.scale[0], transform.scale[1], transform.scale[2]);

                       glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
                       glm::mat4 rm = glm::toMat4(rot);
                       glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

                       t.setPosition(tl);
                       t.setRotation(rot);
                       t.setScale(sc);

                       glm::mat4 result = tm * rm * sm;
                       return result;
                   },
                   [&](fastgltf::math::fmat4x4 matrix) {
                       glm::mat4 glmMatrix;
                       // TODO: Implement conversion from fmat4x4 to glm::mat4
                       glmMatrix = glm::make_mat4(&matrix[0][0]); // Assuming fmat4x4 is a 4x4 float array.
                       t         = Transform(glmMatrix);
                       return glmMatrix;
                   },
               },
               glNode.transform);

    return t;
}

void loadNode(const fastgltf::Asset &asset, const fastgltf::Node &node, SceneNode &sceneNode)
{
    sceneNode.name          = node.name.c_str();
    sceneNode.meshIndex     = node.meshIndex.has_value() ? (int)node.meshIndex.value() : -1;
    sceneNode.lightIndex    = node.lightIndex.has_value() ? (int)node.lightIndex.value() : -1;
    sceneNode.cameraIndex   = node.cameraIndex.has_value() ? (int)node.cameraIndex.value() : -1;
    sceneNode.transform     = gl2Transform(node);
    sceneNode.skeletonIndex = node.skinIndex.has_value() ? (int)node.skinIndex.value() : -1;

    sceneNode.children.resize(node.children.size());
    for (auto i = 0; i < node.children.size(); i++)
    {
        const auto &childNode = asset.nodes[node.children[i]];
        auto       &childPtr  = sceneNode.children[i];
        childPtr              = std::make_unique<SceneNode>();
        loadNode(asset, childNode, *childPtr);
    }
}

Skeleton loadSkeleton(fastgltf::Asset &asset, fastgltf::Skin &skin, std::unordered_map<int, qTypes::JointId> &jointIds)
{
    const auto            &inverseBindAccessor       = asset.accessors[skin.inverseBindMatrices.value()];
    const auto             packedInverseBindMatrices = getPackedBuffer<glm::mat4>(asset, inverseBindAccessor);
    std::vector<glm::mat4> inverseBindMatrices(inverseBindAccessor.count);
    inverseBindMatrices.assign(packedInverseBindMatrices.begin(), packedInverseBindMatrices.end());

    // Prepare the skeleton structure
    const auto jointCount = skin.joints.size();
    Skeleton   skeleton;
    skeleton.joints.reserve(jointCount);
    skeleton.inverseMatricies = inverseBindMatrices;
    skeleton.jointNames.resize(jointCount);

    // Map GLTF node indices to joint IDs
    jointIds.reserve(jointCount);

    { // Load joint data
        JointId currentJointId{0};
        for (const auto &nodeIdx : skin.joints)
        {
            // Map node index to joint ID
            jointIds.emplace(nodeIdx, currentJointId);

            // Access the joint's node data
            const auto &jointNode = asset.nodes[nodeIdx];

            // Store the joint's name and local transformation
            skeleton.jointNames[currentJointId] = jointNode.name;
            skeleton.joints.push_back(Joint{
                .id        = currentJointId,
                .transform = gl2Transform(jointNode),
            });

            ++currentJointId;
        }
    }

    { // Build the joint hierarchy
        skeleton.hierachy.resize(jointCount);
        for (JointId currentJointId = 0; currentJointId < skeleton.joints.size(); ++currentJointId)
        {
            const auto &jointNode = asset.nodes[skin.joints[currentJointId]];

            // Link child joints to the current joint
            for (const auto &childNodeIdx : jointNode.children)
            {
                const auto childJointId = jointIds.at(childNodeIdx);
                skeleton.hierachy[currentJointId].child.push_back(childJointId);
            }
        }
    }

    // Return the constructed skeleton
    return skeleton;
}

std::tuple<float, float> gl2MinMax(fastgltf::Accessor &accessor)
{
    double min, max;
    std::visit(fastgltf::visitor{
                   [&](auto &arg) {},
                   [&](std::pmr::vector<double> vec) { max = vec[0]; },
                   [&](std::pmr::vector<int64_t> vec) { max = vec[0]; },
               },
               accessor.max);
    std::visit(fastgltf::visitor{
                   [&](auto &arg) {},
                   [&](std::pmr::vector<double> vec) { min = vec[0]; },
                   [&](std::pmr::vector<int64_t> vec) { min = vec[0]; },
               },
               accessor.min);
    return {(float)min, (float)max};
}

std::unordered_map<std::string, Animation> loadAnimation(fastgltf::Asset                          &asset,
                                                         std::unordered_map<int, qTypes::JointId> &jointIds,
                                                         Skeleton                                 &skeleton)
{
    std::unordered_map<std::string, Animation> animations(asset.animations.size());

    for (const auto &glanim : asset.animations)
    {
        std::string animName    = std::string(glanim.name.c_str());
        auto       &animation   = animations[animName];
        animation.animationName = animName;

        const auto jointCount = skeleton.joints.size();
        animation.jointTracks.resize(jointCount);

        for (const auto &channel : glanim.channels)
        {
            const auto &sampler = glanim.samplers[channel.samplerIndex];

            auto      &timeAccessor = asset.accessors[sampler.inputAccessor];
            const auto timeStamps   = getPackedBuffer<float>(asset, timeAccessor);

            auto [min, max]             = gl2MinMax(timeAccessor);
            animation.animationDuration = static_cast<float>(max - min);
            auto &dur                   = animation.animationDuration;
            if (animation.animationDuration == 0)
            {
                continue; // Skip animations with no duration
            }

            if (channel.path == fastgltf::AnimationPath::Weights)
            {
                continue;
            }

            const auto nodeIndex = channel.nodeIndex.value();
            const auto jointId   = jointIds.at(nodeIndex);

            const auto &outputAccessor = asset.accessors[sampler.outputAccessor];
            assert(outputAccessor.componentType == fastgltf::ComponentType::Float);

            if (channel.path == fastgltf::AnimationPath::Translation)
            {
                const auto translationKeys = getPackedBuffer<glm::vec3>(asset, outputAccessor);
                auto      &pos             = animation.jointTracks[jointId].positions;

                if (translationKeys.size() == 2 && translationKeys[0] == translationKeys[1])
                {
                    pos.push_back(translationKeys[0]);
                }
                else
                {
                    if (jointId == 0)
                    {
                        assert(translationKeys.size() != 2);
                    }
                    pos.reserve(translationKeys.size());
                    for (const auto &key : translationKeys)
                    {
                        pos.push_back(key);
                    }
                }
            }
            else if (channel.path == fastgltf::AnimationPath::Rotation)
            {
                const auto rotationKeys = getPackedBuffer<glm::vec4>(asset, outputAccessor);
                auto      &rotation     = animation.jointTracks[jointId].rotations;

                if (rotationKeys.size() == 2 && rotationKeys[0] == rotationKeys[1])
                {
                    const auto     &rotationVec = rotationKeys[0];
                    const glm::quat rotationQuat{rotationVec.w, rotationVec.x, rotationVec.y, rotationVec.z};
                    rotation.push_back(rotationQuat);
                }
                else
                {
                    rotation.reserve(rotationKeys.size());
                    for (const auto &rotationVec : rotationKeys)
                    {
                        const glm::quat rotationQuat{rotationVec.w, rotationVec.x, rotationVec.y, rotationVec.z};
                        rotation.push_back(rotationQuat);
                    }
                }
            }
            else if (channel.path == fastgltf::AnimationPath::Scale)
            {
                const auto scaleKeys  = getPackedBuffer<glm::vec3>(asset, outputAccessor);
                auto      &scaleTrack = animation.jointTracks[jointId].scales;

                if (scaleKeys.size() == 2 && scaleKeys[0] == scaleKeys[1])
                {
                    scaleTrack.push_back(scaleKeys[0]);
                }
                else
                {
                    scaleTrack.reserve(scaleKeys.size());
                    for (const auto &key : scaleKeys)
                    {
                        scaleTrack.push_back(key);
                    }
                }
            }
            else
            {
                assert(false && "Unexpected target_path in animation channel");
            }
        }
    }

    return animations;
}

Scene glTFUtil::loadGltfFile(std::filesystem::path path, GPUDevice &device)
{

    fmt::println("Start loading gltf {}", path.string());

    static constexpr auto supportedExtensions = fastgltf::Extensions::KHR_lights_punctual |
                                                fastgltf::Extensions::KHR_materials_emissive_strength |
                                                fastgltf::Extensions::KHR_materials_variants;
    fastgltf::Parser parser{supportedExtensions};
    constexpr auto   gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble |
                                 fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

    auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);

    auto             load  = parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);
    fastgltf::Asset &asset = load.get();

    Scene scene{.path = path};
    scene.models.reserve(asset.meshes.size());
    const auto fileDir   = path.parent_path();
    const auto mainScene = asset.scenes[asset.defaultScene.value()];

    auto materialPool = MaterialPool::GetInstance();
    auto meshPool     = MeshPool::GetInstance();

    std::unordered_map<std::size_t, qTypes::MaterialId> materialMap;
    { // load materials

        for (auto index = 0; index < asset.materials.size(); index++)
        {
            // get material
            auto &gltfMaterial = asset.materials[index];
            auto  material     = std::move(loadMaterial(device, asset, fileDir, gltfMaterial));
            auto  materialId   = materialPool->addMaterial(device, material);
            materialMap.emplace(index, materialId);
        }
    }

    { // load meshes
        // alloc space for meshes
        scene.models.reserve(asset.meshes.size());
        for (auto &mesh : asset.meshes)
        {
            ModelPool model;
            model.primitiveMeshes.resize(mesh.primitives.size());
            model.primitiveMaterials.resize(mesh.primitives.size());
            for (auto index = 0; index < mesh.primitives.size(); index++)
            {
                const auto &primitive = mesh.primitives[index];
                MeshProps   meshProps = loadPrimitives(asset, mesh.name.c_str(), primitive);

                if (meshProps.indices.empty())
                    continue;

                // upload to gpu
                auto materialID = materialPool->getPlaceholder();
                if (primitive.materialIndex.has_value())
                {
                    materialID = materialMap.at(primitive.materialIndex.value());
                }

                const auto meshID               = meshPool->addMesh(device, meshProps);
                model.primitiveMeshes[index]    = meshID;
                model.primitiveMaterials[index] = materialID;

                scene.meshProps.emplace(meshID, std::move(meshProps));
            }
            scene.models.push_back(std::move(model));
        }
    }

    { // load light
        scene.lights.reserve(asset.lights.size());
        for (const auto &light : asset.lights)
        {
            scene.lights.push_back(loadLight(light));
        }
    }

    { // animations and skeleton
        std::unordered_map<int, qTypes::JointId> jointIds;
        scene.skeletons.reserve(asset.skins.size());
        for (auto &skin : asset.skins)
        {
            scene.skeletons.push_back(loadSkeleton(asset, skin, jointIds));
        }

        if (!asset.skins.empty())
        {
            assert(asset.skins.size() == 1); // for now only one skeleton supported
            scene.animations = loadAnimation(asset, jointIds, scene.skeletons[0]);
        }
    }

    { // load nodes
        scene.nodes.resize(mainScene.nodeIndices.size());
        for (auto i = 0; i < mainScene.nodeIndices.size(); i++)
        {
            const auto &glNode = asset.nodes.at(mainScene.nodeIndices.at(i));
            // don't know but it works somehow
            if (glNode.children.size() == 2)
            {
                const auto &node1 = asset.nodes[glNode.children.at(0)];
                const auto &node2 = asset.nodes[glNode.children.at(1)];

                if ((node1.meshIndex.has_value() && node1.skinIndex.has_value()) ||
                    (node2.meshIndex.has_value() && node2.skinIndex.has_value()))
                {
                    const auto &meshNode = node1.meshIndex.has_value() ? node1 : node2;
                    auto       &nodePtr  = scene.nodes[i];
                    nodePtr              = std::make_unique<SceneNode>();
                    loadNode(asset, meshNode, *nodePtr);

                    const auto parentTransform = gl2Transform(glNode);
                    nodePtr->transform         = parentTransform * nodePtr->transform;
                    continue;
                }
            }

            auto &nodePtr = scene.nodes[i];
            nodePtr       = std::make_unique<SceneNode>();
            loadNode(asset, glNode, *nodePtr);
        }
    }

    return scene;
}