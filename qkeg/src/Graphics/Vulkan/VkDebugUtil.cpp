#include "Vulkan/VkDebugUtil.h"

void cmdBeginLabel(VkCommandBuffer cmd, const char *label, glm::vec4 color) {}

void cmdEndLabel(VkCommandBuffer cmd) {}

void addDebugLabel(VkDevice device, VkImage image, const char *label) {}

void addDebugLabel(VkDevice device, VkImageView imageView, const char *label) {}

void addDebugLabel(VkDevice device, VkShaderModule shaderModule, const char *label) {}

void addDebugLabel(VkDevice device, VkPipeline pipeline, const char *label) {}

void addDebugLabel(VkDevice device, VkPipelineLayout layout, const char *label) {}

void addDebugLabel(VkDevice device, VkBuffer buffer, const char *label) {}

void addDebugLabel(VkDevice device, VkSampler sampler, const char *label) {}
