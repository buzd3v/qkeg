
#include "Vulkan/BindlessDescriptor.h"
#include <Vulkan/VkUtil.h>
#include <array>

static const std::uint32_t k_max_bindless_resouces = 16536;
static const std::uint32_t k_max_sampler           = 32;
static const std::uint32_t k_texture_binding       = 0;
static const std::uint32_t k_sampler_binding       = 1;

void BindlessDescriptor::init(VkDevice device, float maxAnisotropy)
{
    { // create desc pool
        const auto poolSizeBindless = std::array<VkDescriptorPoolSize, 2>{{
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, k_max_bindless_resouces},
            {VK_DESCRIPTOR_TYPE_SAMPLER, k_max_sampler},
        }};

        const auto poolInfo = VkDescriptorPoolCreateInfo{
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags         = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT,
            .maxSets       = k_max_bindless_resouces * poolSizeBindless.size(),
            .poolSizeCount = (std::uint32_t)poolSizeBindless.size(),
            .pPoolSizes    = poolSizeBindless.data(),
        };

        VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &bindlessPool));
    }

    { // Descriptor Layout Creation

        VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                                                 VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
                                                 VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        const auto bindingFlags = std::array{bindlessFlags, bindlessFlags};

        const auto bindings = std::array<VkDescriptorSetLayoutBinding, 2>{{
            {
                .binding         = k_texture_binding,
                .descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = k_max_bindless_resouces,
                .stageFlags      = VK_SHADER_STAGE_ALL,
            },
            {
                .binding         = k_sampler_binding,
                .descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = k_max_sampler,
                .stageFlags      = VK_SHADER_STAGE_ALL,
            },
        }};

        const auto desSetFlagsInfo = VkDescriptorSetLayoutBindingFlagsCreateInfo{
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .bindingCount  = (std::uint32_t)bindingFlags.size(),
            .pBindingFlags = bindingFlags.data(),
        };

        const auto desSetLayoutInfo = VkDescriptorSetLayoutCreateInfo{
            .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext        = &desSetFlagsInfo,
            .flags        = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT,
            .bindingCount = (std::uint32_t)bindings.size(),
            .pBindings    = bindings.data(),
        };

        VK_CHECK(vkCreateDescriptorSetLayout(device, &desSetLayoutInfo, nullptr, &bindlessLayout));
    }

    { // alloc descriptor set

        auto maxBinding = std::array<uint32_t, 2>{k_max_bindless_resouces - 1, k_max_sampler - 1};

        const auto countInfo = VkDescriptorSetVariableDescriptorCountAllocateInfo{
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
            .descriptorSetCount = (std::uint32_t)maxBinding.size(),
            .pDescriptorCounts  = maxBinding.data(),
        };

        const auto allocInfo = VkDescriptorSetAllocateInfo{
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext              = &countInfo,
            .descriptorPool     = bindlessPool,
            .descriptorSetCount = 1,
            .pSetLayouts        = &bindlessLayout,
        };

        VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &bindlessSet));
    }

    { // init default sampler
        static const std::uint32_t nearestSamplerId   = 0;
        static const std::uint32_t linearSamplerId    = 1;
        static const std::uint32_t shadowMapSamplerId = 2;

        { // nearest sampler
            const auto createInfo = VkSamplerCreateInfo{
                .sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
            };
            VK_CHECK(vkCreateSampler(device, &createInfo, nullptr, &nearestSampler));
            addSampler(device, nearestSamplerId, nearestSampler);
        }

        { // linear sampler
            const auto createInfo = VkSamplerCreateInfo{
                .sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .magFilter        = VK_FILTER_LINEAR,
                .minFilter        = VK_FILTER_LINEAR,
                .mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .anisotropyEnable = VK_TRUE,
                .maxAnisotropy    = maxAnisotropy,
            };
            VK_CHECK(vkCreateSampler(device, &createInfo, nullptr, &linearSampler));
            addSampler(device, linearSamplerId, linearSampler);
        }
        { // nearest sampler
            const auto createInfo = VkSamplerCreateInfo{
                .sType         = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .magFilter     = VK_FILTER_LINEAR,
                .minFilter     = VK_FILTER_LINEAR,
                .compareEnable = VK_TRUE,
                .compareOp     = VK_COMPARE_OP_GREATER_OR_EQUAL,
            };
            VK_CHECK(vkCreateSampler(device, &createInfo, nullptr, &shadowMapSampler));
            addSampler(device, shadowMapSamplerId, shadowMapSampler);
        }
    }
}

void BindlessDescriptor::cleanUp(VkDevice device)
{
    vkDestroySampler(device, nearestSampler, nullptr);
    vkDestroySampler(device, linearSampler, nullptr);
    vkDestroySampler(device, shadowMapSampler, nullptr);

    vkDestroyDescriptorSetLayout(device, bindlessLayout, nullptr);
    vkDestroyDescriptorPool(device, bindlessPool, nullptr);
}

void BindlessDescriptor::addImage(VkDevice device, std::uint32_t id, VkImageView imageView)
{
    const auto imageInfo = VkDescriptorImageInfo{
        .imageView   = imageView,
        .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    };
    const auto writeset = VkWriteDescriptorSet{
        .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet          = bindlessSet,
        .dstBinding      = k_texture_binding,
        .dstArrayElement = id,
        .descriptorCount = 1,
        .descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo      = &imageInfo,
    };

    vkUpdateDescriptorSets(device, 1, &writeset, 0, nullptr);
}

void BindlessDescriptor::addSampler(VkDevice device, std::uint32_t id, VkSampler sampler)
{
    const auto imageInfo = VkDescriptorImageInfo{
        .sampler     = sampler,
        .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    };
    const auto writeset = VkWriteDescriptorSet{
        .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet          = bindlessSet,
        .dstBinding      = k_sampler_binding,
        .dstArrayElement = id,
        .descriptorCount = 1,
        .descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo      = &imageInfo,
    };

    vkUpdateDescriptorSets(device, 1, &writeset, 0, nullptr);
}
