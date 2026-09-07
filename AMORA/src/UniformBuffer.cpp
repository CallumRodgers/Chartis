////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include "UniformBuffer.hpp"

#include "vulkan_utils.hpp"

using namespace amora;

template<class UBO> UniformBuffer<UBO>::UniformBuffer(const VulkanContext& context) {

    mBufferObjectSize = sizeof(UBO);

    /////////////////////
    // CREATING BUFFER //
    /////////////////////

    const vk::BufferCreateInfo bufferCreateInfo {
        .size = mBufferObjectSize,
        .usage = vk::BufferUsageFlagBits::eUniformBuffer,
        .sharingMode = vk::SharingMode::eExclusive
    };
    mBuffer = vk::raii::Buffer(context.getDevice(), bufferCreateInfo);

    ///////////////////////
    // ALLOCATING MEMORY //
    ///////////////////////

    const vk::MemoryRequirements memoryRequirements = mBuffer.getMemoryRequirements();
    const vk::MemoryAllocateInfo memoryAllocateInfo {
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = vulkan_utils::findMemoryType(
            context.getPhysicalDevice(),
            memoryRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        )
    };
    mBufferMemory = vk::raii::DeviceMemory(context.getDevice(), memoryAllocateInfo);

    ////////////////////////////////
    // BINDING AND MAPPING MEMORY //
    ////////////////////////////////

    mBuffer.bindMemory(*mBufferMemory, 0);
    mBufferMap = mBufferMemory.mapMemory(0, bufferCreateInfo.size);
}

template<class UBO> void UniformBuffer<UBO>::uploadData(const UBO& bufferData) const {
    std::memcpy(mBufferMap, &bufferData, sizeof(bufferData));
}

template<class UBO> void UniformBuffer<UBO>::unmapMemory() {
    mBufferMemory.unmapMemory();
    mBufferMap = nullptr;
}
