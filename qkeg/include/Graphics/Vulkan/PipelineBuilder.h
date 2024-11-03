// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
// clang-format on

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