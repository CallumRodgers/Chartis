//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "Texture.hpp"

#include "vulkan_utils.hpp"

using namespace amora;

Texture::Texture(const VulkanContext& context, int width, int height, unsigned char* pixels) {
    /////////////////////////////
    // CREATING STAGING BUFFER //
    /////////////////////////////

    const vk::DeviceSize textureSize = static_cast<vk::DeviceSize>(width * height * 4);

    const vk::BufferCreateInfo stageBufferCreateInfo {
        .size = textureSize,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .sharingMode = vk::SharingMode::eExclusive,
    };
    const auto stagingBuffer = vk::raii::Buffer(context.getDevice(), stageBufferCreateInfo);

    const vk::MemoryRequirements memoryRequirements = stagingBuffer.getMemoryRequirements();
    const vk::MemoryAllocateInfo memoryAllocateInfo {
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = vulkan_utils::findMemoryType(
            context.getPhysicalDevice(),
            memoryRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        )
    };
    const auto stagingBufferMemory = vk::raii::DeviceMemory(context.getDevice(), memoryAllocateInfo);
    stagingBuffer.bindMemory(*stagingBufferMemory, 0);

    void* data = stagingBufferMemory.mapMemory(0, textureSize);
    std::memcpy(data, pixels, textureSize);
    stagingBufferMemory.unmapMemory();

    ////////////////////
    // CREATING IMAGE //
    ////////////////////

    const vk::ImageCreateInfo imageCreateInfo {
        .imageType = vk::ImageType::e2D,
        .format = vk::Format::eR8G8B8A8Srgb,
        .extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined
    };
    mImage = vk::raii::Image(context.getDevice(), imageCreateInfo);

    const vk::MemoryRequirements imageMemRequirements = mImage.getMemoryRequirements();
    const vk::MemoryAllocateInfo imageMemAllocateInfo {
        .allocationSize = imageMemRequirements.size,
        .memoryTypeIndex = vulkan_utils::findMemoryType(
            context.getPhysicalDevice(),
            memoryRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eDeviceLocal
        )
    };
    mImageMemory = vk::raii::DeviceMemory(context.getDevice(), imageMemAllocateInfo);
    mImage.bindMemory(*mImageMemory, 0);
}
