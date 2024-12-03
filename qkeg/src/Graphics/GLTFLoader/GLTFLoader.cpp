#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include "Common.h"
#include "GLTFLoader/GLTFLoader.h"
#include "GPUDevice.h"
#include "Renderer/Scene.h"

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
std::filesystem::path getNormalMapTexturePath(const std::filesystem::path fileDir, const fastgltf::Asset &asset,
                                              const fastgltf::Material &material)
{
    const auto  texIndex = material.normalTexture.value().textureIndex;
    const auto &texId    = asset.textures[texIndex];
    const auto &image    = asset.images[texId.imageIndex.value()];

    return fileDir / image.name;
}
std::filesystem::path getEmissiveTexturePath(const std::filesystem::path fileDir, const fastgltf::Asset &asset,
                                             const fastgltf::Material &material)
{
    const auto  texIndex = material.emissiveTexture.value().textureIndex;
    const auto &texId    = asset.textures[texIndex];
    const auto &image    = asset.images[texId.imageIndex.value()];

    return fileDir / image.name;
}

std::filesystem::path getDiffuseTexturePath(const std::filesystem::path fileDir, const fastgltf::Asset &asset,
                                            const fastgltf::Material &material)
{
    const auto  texIndex = material.pbrData.baseColorTexture.value().textureIndex;
    const auto &texId    = asset.textures[texIndex];
    const auto &image    = asset.images[texId.imageIndex.value()];

    return fileDir / image.name;
}

std::filesystem::path getMetallicRoughnessTexture(const std::filesystem::path fileDir, const fastgltf::Asset &asset,
                                                  const fastgltf::Material &material)
{
    const auto  texIndex = material.pbrData.metallicRoughnessTexture.value().textureIndex;
    const auto &texId    = asset.textures[texIndex];
    const auto &image    = asset.images[texId.imageIndex.value()];

    return fileDir / image.name;
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
            return accessorID;
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

// std::span<uint16_t> getPackedSpan(const fastgltf::Asset &asset, const fastgltf::Accessor &accessor)
// {
//     const auto &bufferView = asset.bufferViews[accessor.bufferViewIndex.value()];
//     const auto  byteStride = bufferView.byteStride.value();
//     assert(byteStride == sizeof(uint16_t)); // checking for tightly packed buffer

//     const auto &buffer = asset.buffers.at(bufferView.bufferIndex);
//     uint16_t   *data   = reinterpret_cast<uint16_t
//     *>(std::get<fastgltf::sources::ByteView>(buffer.data).bytes.first()); return std::span{data, accessor.count};
// }

MeshProps loadPrimitives(const fastgltf::Asset &asset, const std::string name, const fastgltf::Primitive &primitive)
{
    MeshProps props{.name = name};

    if (primitive.indicesAccessor.has_value())
    {

        auto &indicesAcessor = asset.accessors[primitive.indicesAccessor.value()];
        auto  indices        = getPackedBuffer<uint32_t>(asset, indicesAcessor);
        // props.indices.assign(indices.begin(), indices.end());
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

            // skeleton
            //  for (auto i = 0; i < tangents.size(); i++)
            //  {
            //      props.vertices[i].tangent = tangents[i];
            //  }
        }
    }
    return props;
}

Scene glTFUtil::loadGltfFile(std::filesystem::path path, GPUDevice &device)
{

    fastgltf::Asset asset = loadglTF(path);
    Scene           scene{.path = path};
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

    return scene;
}