#pragma once
#include "Buffers.h"
#include "Common.h"
#include "Light.h"
#include "Math/Transform.h"
#include "Mesh/MeshDrawProps.h"
#include "Pipeline/CubemapPipeline.h"
#include "Pipeline/GradientPipeline.h"
#include "Pipeline/MeshPipeline.h"
#include "Pipeline/SkeletonPipeline.h"
#include "Vulkan/GPUImage.h"
#include <glm/glm.hpp>
#include <singleton_atomic.hpp>
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

        VkDeviceAddress lightsAddress;
        uint32_t        lightCounts;
        int32_t         sunlightIndex;

        VkDeviceAddress materialAddress;
    };

  public:
    void        init(GPUDevice &device, const glm::ivec2 &drawImageSize);
    void        draw(GPUDevice &device, VkCommandBuffer cmd, Camera &camera, const SceneProps &props);
    void        cleanUp(GPUDevice &device);
    void        collectDataBegin(GPUDevice &device);
    void        collectDataEnd() {}
    std::size_t addJointMatricies(GPUDevice &device, std::span<glm::mat4> jointMat);

  public:
    const GPUImage &getDrawImage(GPUDevice &device);
    const GPUImage &getDepthImage(GPUDevice &device);
    VkFormat        getDrawImageFormat() const;
    VkFormat        getDepthImageFormat() const;

  public:
    // these function below to add data of each kind for rendering path
    void addMesh(qTypes::MeshId meshId, qTypes::MaterialId matId, glm::mat4 &transform, bool castShadow = true);
    void addLight(Light &light, Transform &transform);
    void addSkeletonMesh(qTypes::MeshId meshId, qTypes::MaterialId matId, glm::mat4 &transform, SkinningMesh &skinMesh,
                         size_t jointMatStartIdx);

  public:
    void setCubemapImage(ImageId image) { cubemapPipeline.setCubemap(image); }

  private:
    void createDrawImage(GPUDevice &device, glm::ivec2 renderSize, bool haveCreate);

    MeshPipeline     meshPipeline{};
    SkeletonPipeline skeletonPipeline{};
    CubemapPipeline  cubemapPipeline{};
    GradientPipeline gradPipeline{};

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
    Buffers                    lightBuffers;
    std::vector<LightProps>    lightProps; // this light props is data to send to GPU
    std::vector<MeshDrawProps> meshDrawProps;
    int                        sunlightIndex = -1;
};
