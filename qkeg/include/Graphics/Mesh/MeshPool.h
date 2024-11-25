#pragma once

#include "GPUMesh.h"
#include <singleton_atomic.hpp>

using namespace qTypes;

class GPUDevice;
class MeshPool : public SingletonAtomic<MeshPool>
{
  public:
    void     cleanUp(GPUDevice &device);
    MeshId   addMesh(GPUDevice &device, MeshProps &mesh);
    GPUMesh &getMesh(MeshId id);

  private:
    void                 loadMesh(GPUDevice &device, GPUMesh &mesh, MeshProps &data);
    std::vector<GPUMesh> meshes;
};