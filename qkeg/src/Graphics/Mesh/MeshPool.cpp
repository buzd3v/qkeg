#include "Mesh/MeshPool.h"
#include "Vulkan/GPUBuffer.h"
#include "Vulkan/VkExcutor.h"
void MeshPool::cleanUp(GPUDevice &device)
{
    for (auto mesh : meshes)
    {
        mesh.indexBuffer.cleanup(device);
        mesh.vertexBuffer.cleanup(device);
    }
}

MeshId MeshPool::addMesh(GPUDevice &device, MeshProps &mesh)
{
    GPUMesh gpuMesh{
        .numIndices  = (uint32_t)mesh.indices.size(),
        .numVertices = (uint32_t)mesh.vertices.size(),
    };

    // incase calculate bounding sphere
    std::vector<glm::vec3> pos(mesh.vertices.size());
    for (uint32_t i = 0; i < pos.size(); i++)
    {
        pos[i] = mesh.vertices[i].position;
    }

    auto id = (uint32_t)meshes.size();
    loadMesh(device, gpuMesh, mesh);
    meshes.push_back(gpuMesh);
    return id;
}

GPUMesh &MeshPool::getMesh(MeshId id)
{
    return meshes.at(id);
}

void MeshPool::loadMesh(GPUDevice &device, GPUMesh &mesh, MeshProps &data)
{
    const auto indexBufferSize  = data.indices.size() * sizeof(uint32_t);
    const auto vertexBufferSize = data.vertices.size() * sizeof(MeshProps::Vertex);

    mesh.indexBuffer = GPUBufferBuilder{device, indexBufferSize}
                           .setBufferUsageFlags(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
                           .build();
    mesh.vertexBuffer = GPUBufferBuilder{device, vertexBufferSize}
                            .setBufferUsageFlags(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                 VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
                            .build();

    auto stagingBuffer = GPUBufferBuilder{device, indexBufferSize + vertexBufferSize}
                             .setBufferUsageFlags(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
                             .build();

    // copy data to staging buffer
    void *d = stagingBuffer.info.pMappedData;
    memcpy(d, data.vertices.data(), vertexBufferSize);
    memcpy((char *)d + vertexBufferSize, data.indices.data(), indexBufferSize);

    auto *excutor = VkExcutor::GetInstance();
    excutor->submit([&](VkCommandBuffer cmd) {
        const auto vertexCopy = VkBufferCopy{
            .srcOffset = 0,
            .dstOffset = 0,
            .size      = vertexBufferSize,
        };
        const auto indexcopy = VkBufferCopy{
            .srcOffset = vertexBufferSize,
            .dstOffset = 0,
            .size      = indexBufferSize,
        };

        vkCmdCopyBuffer(cmd, stagingBuffer.buffer, mesh.vertexBuffer.buffer, 1, &vertexCopy);
        vkCmdCopyBuffer(cmd, stagingBuffer.buffer, mesh.indexBuffer.buffer, 1, &indexcopy);
    });

    stagingBuffer.cleanup(device);

    if (mesh.hasSkeleton) // only skeletal mesh
    {
        // create skinning data buffer
        const auto skinningDataSize = data.vertices.size() * sizeof(MeshProps::SkinningProps);
        mesh.skinningDataBuffer =
            GPUBufferBuilder(device, skinningDataSize)
                .setBufferUsageFlags(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
                .build();

        auto staging =
            GPUBufferBuilder(device, skinningDataSize).setBufferUsageFlags(VK_BUFFER_USAGE_TRANSFER_SRC_BIT).build();

        // copy data
        void *dataToCopy = staging.info.pMappedData;
        memcpy(dataToCopy, data.skinningProps.data(), skinningDataSize);

        auto excutor = VkExcutor::GetInstance();
        excutor->submit([&](VkCommandBuffer cmd) {
            const auto vertexCopy = VkBufferCopy{
                .srcOffset = 0,
                .dstOffset = 0,
                .size      = skinningDataSize,
            };
            vkCmdCopyBuffer(cmd, staging.buffer, mesh.skinningDataBuffer.buffer, 1, &vertexCopy);
        });

        staging.cleanup(device);
    }
}
