//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "DescriptorSetData.hpp"

using namespace amora;

DescriptorSetData::DescriptorSetData(const std::vector<vk::DescriptorSetLayout>& layouts, const DescriptorAllocator& allocator) {
    mDescriptorSets = allocator.allocate(layouts);
}

void DescriptorSetData::linkToBuffer(const vk::DescriptorBufferInfo& bufferInfo, const uint32_t dstSet, const uint32_t dstBinding) const {
    const vk::raii::DescriptorSet& set = mDescriptorSets[dstSet];

    const vk::WriteDescriptorSet writeDescriptorSet {
        .dstSet = set,
        .dstBinding = dstBinding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo = &bufferInfo
    };
    set.getDevice().updateDescriptorSets(writeDescriptorSet, {});
}


