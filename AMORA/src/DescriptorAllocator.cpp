//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "DescriptorAllocator.hpp"

#include <AMORA/config.hpp>

using namespace amora;

DescriptorAllocator::DescriptorAllocator(const VulkanContext& context) : mDevice(context.getDevice()) {
    constexpr uint32_t maxSets = config::rendering::MAX_FRAMES_IN_FLIGHT * (config::rendering::MAX_BINDING_SET_INDEX + 1);

    mPoolSize = {
        .type = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = maxSets * (config::rendering::MAX_BINDING_INDEX + 1)
    };

    const vk::DescriptorPoolCreateInfo poolCreateInfo {
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = maxSets,
        .poolSizeCount = 1,
        .pPoolSizes = &mPoolSize
    };
    mPool = vk::raii::DescriptorPool(context.getDevice(), poolCreateInfo);
}

std::vector<vk::raii::DescriptorSet> DescriptorAllocator::allocate(const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts) const {
    const vk::DescriptorSetAllocateInfo allocateInfo {
        .descriptorPool = mPool,
        .descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
        // This mess: One * is for dereferencing data(), another for the DescriptorSetLayout* pointer,
        // another for going back from the ::raii namespace, and finally a & for taking the pointer of that.
        .pSetLayouts = descriptorSetLayouts.data()
    };
    return mDevice.allocateDescriptorSets(allocateInfo);
}
