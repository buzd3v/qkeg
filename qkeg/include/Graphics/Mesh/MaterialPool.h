#pragma once

#include "GPUMaterial.h"
#include "Vulkan/GPUBuffer.h"
#include "singleton_atomic.hpp"
class GPUDevice;

class MaterialPool : public SingletonAtomic<MaterialPool>
{
  public:
    void init(GPUDevice &device);
    void cleanUp(GPUDevice &device);

  public:
    [[nodiscard]] qTypes::MaterialId addMaterial(GPUDevice &device, GPUMaterial &material);
    [[nodiscard]] qTypes::MaterialId requestFreeId();
    [[nodiscard]] qTypes::MaterialId getPlaceholder();

    GPUMaterial getMaterial(qTypes::MaterialId id);

  private:
    std::vector<GPUMaterial> materials;

    GPUBuffer          materialBuffer;
    qTypes::MaterialId placeHolderId{qTypes::NULL_MATERIAL_ID};
    ImageId            defaultTextureId{qTypes::NULL_IMAGE_ID};

    static const uint32_t MAX_MATERIALS = 10000;
};