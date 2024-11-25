// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
// clang-format on
#include <vector>
class ComputePipelineBuilder
{
  public:
    ComputePipelineBuilder(VkPipelineLayout pipelineLayout);
    ComputePipelineBuilder &setShaderModule(VkShaderModule module);

    VkPipeline build(VkDevice device);

  private:
    VkPipelineLayout pipelineLayout;
    VkShaderModule   module;
};

class GraphicsPipelineBuilder
{
  public:
    GraphicsPipelineBuilder(VkPipelineLayout pipelineLayout);

    GraphicsPipelineBuilder &setShaderModule(VkShaderModule vertexShader   = VK_NULL_HANDLE,
                                             VkShaderModule fragmentShader = VK_NULL_HANDLE,
                                             VkShaderModule geometryShader = VK_NULL_HANDLE);
    GraphicsPipelineBuilder &setInputTopology(VkPrimitiveTopology topo);
    GraphicsPipelineBuilder &setPolygonMode(VkPolygonMode mode);
    GraphicsPipelineBuilder &setCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
    GraphicsPipelineBuilder &enableCulling(bool shouldEnable);
    GraphicsPipelineBuilder &setMultiSampling(VkSampleCountFlagBits flags);
    GraphicsPipelineBuilder &disableBlending();
    GraphicsPipelineBuilder &enableBlending(VkBlendOp     blendOp             = VK_BLEND_OP_ADD,
                                            VkBlendFactor srcBlendFactor      = VK_BLEND_FACTOR_SRC_ALPHA,
                                            VkBlendFactor dstBlendFactor      = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                                            VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                                            VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
    GraphicsPipelineBuilder &setColorAttachmentFormat(VkFormat format);
    GraphicsPipelineBuilder &setDepthFormat(VkFormat format);
    GraphicsPipelineBuilder &enableDepthTest(bool depthWriteEnable, VkCompareOp op);
    GraphicsPipelineBuilder &enableDepthClamp();
    GraphicsPipelineBuilder &disableDepthTest();
    GraphicsPipelineBuilder &enableDynamicDepth();
    GraphicsPipelineBuilder &enableDepthBias(float constantFactor, float slopeFactor);

    VkPipeline build(VkDevice device);

  private:
    VkPipelineLayout pipelineLayout;
    bool             dynamicDepth{false};

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineInputAssemblyStateCreateInfo       inputAsssembly;
    VkPipelineRasterizationStateCreateInfo       rasterizer;
    VkPipelineColorBlendAttachmentState          colorBlendAttch;
    VkPipelineMultisampleStateCreateInfo         multiSampling;
    VkPipelineDepthStencilStateCreateInfo        depthStencil;
    VkPipelineRenderingCreateInfo                renderInfo;
    VkFormat                                     colorAttachmentFormat;
};