#pragma once
#include <glm/glm.hpp>
#include <singleton_atomic.hpp>

#include "Buffers.h"
#include "Mesh/MeshDrawProps.h"
#include "Pipeline/MeshPipeline.h"
#include "Vulkan/GPUImage.h"

class MeshPool;
class MaterialPool;
class GPUDevice;
class Camera;

class GameRenderer : public SingletonAtomic<GameRenderer>
{
  public:
    struct SceneProps
    {
        Camera             &camera;
        qColor::LinearColor ambientColor;
        float               ambientIntensity;
    };

    // send this data to gpu
    struct SceneData
    {
        // camera data
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
        glm::vec4 cameraPos;

        // ambient
        qColor::Color ambientColor;
        float         ambientIntensity;

        VkDeviceAddress materialAddress;
    };

  public:
    void init(GPUDevice &device, const glm::ivec2 &drawImageSize);
    void draw(GPUDevice &device, VkCommandBuffer cmd, const Camera &camera, const SceneProps &props);
    void cleanUp(GPUDevice &device);

  public:
    GPUImage &getDrawImage(GPUDevice &device) const;
    GPUImage &getDepthImage(GPUDevice &device) const;
    VkFormat  getDrawImageFormat() const;
    VkFormat  getDepthImageFormat() const;

  public:
    // these function below to add data of each kind for rendering path
    void addMesh(qTypes::MeshId meshId, qTypes::MaterialId matId, glm::mat4 &transform, MeshDrawProps props,
                 bool castShadow = true);

  private:
    void createDrawImage(GPUDevice &device, glm::ivec2 renderSize, bool haveCreate);

    MeshPipeline meshPipeline{};

    ImageId drawImageId{qTypes::NULL_IMAGE_ID};
    ImageId depthImageId{qTypes::NULL_IMAGE_ID};
    ImageId resolveImageId{qTypes::NULL_IMAGE_ID};
    ImageId resolveDepthImageId{qTypes::NULL_IMAGE_ID};
    ImageId postFxImageId{qTypes::NULL_IMAGE_ID};

    VkFormat drawImageFormat{VK_FORMAT_R16G16B16A16_SFLOAT};
    VkFormat depthImageFormat{VK_FORMAT_D32_SFLOAT};

    VkSampleCountFlagBits sampleCounts{VK_SAMPLE_COUNT_1_BIT};

    MeshPool     *meshPool;
    MaterialPool *materialPool;
    ImagePool    *imagePool;

    Buffers                    sceneDataBuffers;
    std::vector<MeshDrawProps> meshDrawProps;
};
