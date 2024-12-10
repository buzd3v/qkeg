#include "Vulkan/GPUBuffer.h"
class GPUDevice;

// we will have N-1 staging buffer and 1 buffer sending data to GPU so that we can avoid missing data
class Buffers
{
  public:
    void init(GPUDevice &device, VkBufferUsageFlags flags, size_t nBuffers, size_t bufferSize);
    void cleanup(GPUDevice &device);
    void uploadData(VkCommandBuffer cmd, size_t bufferIndex, void *data, size_t dataSize, size_t offset = 0,
                    bool sync = true);

    GPUBuffer &getBuffer() { return headBuffer; }

  private:
    size_t                 nBuffers{0};   // number of buffer
    size_t                 bufferSize{0}; // size of each buffer
    std::vector<GPUBuffer> stagingBuffers;
    GPUBuffer              headBuffer;
    bool                   isInit{false};
};