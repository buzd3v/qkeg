#include "Mesh/MaterialPool.h"
#include "Vulkan/ImagePool.h"

void MaterialPool::init(GPUDevice &device)
{
    materialBuffer =
        GPUBufferBuilder{device, MAX_MATERIALS * sizeof(MaterialProps)}
            .setBufferUsageFlags(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
            .build();

    auto pool       = ImagePool::GetInstance();
    auto createInfo = GPUImageCreateInfo{
        .format     = VK_FORMAT_R8G8B8A8_UNORM,
        .usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .extent     = VkExtent3D{1, 1, 1},
    };

    defaultTextureId = pool->createImage(createInfo);

    GPUMaterial placeHolder{};
    placeHolderId = addMaterial(device, placeHolder);
}

void MaterialPool::cleanUp(GPUDevice &device)
{
    materialBuffer.cleanup(device);
}

qTypes::MaterialId MaterialPool::addMaterial(GPUDevice &device, GPUMaterial &material)
{
    const auto chooseTexture = [](ImageId imgId, ImageId placeHolder) {
        return imgId != qTypes::NULL_IMAGE_ID ? imgId : placeHolder;
    };

    MaterialProps *props = (MaterialProps *)materialBuffer.info.pMappedData;

    auto whiteTextureId = qTypes::NULL_IMAGE_ID;
    auto id             = requestFreeId();

    assert(id < MAX_MATERIALS);
    props[id] = MaterialProps{
        .baseColor = material.baseColor,
        .metalRoughnessEmissive =
            glm::vec4(material.metalicFactor, material.roughnessFactor, material.emissiveFactor, 0.f),
        .diffuseTex           = chooseTexture(material.diffuseTexture, whiteTextureId),
        .normalTex            = chooseTexture(material.normalMapTexture, defaultTextureId),
        .metallicRoughnessTex = chooseTexture(material.metallicRoughnessTexture, whiteTextureId),
        .emissiveTex          = chooseTexture(material.emissiveTexture, whiteTextureId),
    };

    materials.push_back(std::move(material));
    return id;
}

qTypes::MaterialId MaterialPool::requestFreeId()
{
    return (qTypes::MaterialId)materials.size();
}

qTypes::MaterialId MaterialPool::getPlaceholder()
{
    assert(placeHolderId != qTypes::NULL_IMAGE_ID);
    return placeHolderId;
}

GPUMaterial MaterialPool::getMaterial(qTypes::MaterialId id)
{
    return materials.at(id);
}
