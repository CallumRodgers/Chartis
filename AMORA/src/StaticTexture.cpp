//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "StaticTexture.hpp"

#include "vulkan_utils.hpp"

using namespace amora;

StaticTexture::StaticTexture(const VulkanContext& context, int width, int height, unsigned char* pixels) {
    mWidth = width;
    mHeight = height;

    /////////////////////////////
    // CREATING STAGING BUFFER //
    /////////////////////////////

    const vk::DeviceSize textureSize = static_cast<vk::DeviceSize>(width * height * 4);

    const vk::BufferCreateInfo stageBufferCreateInfo {
        .size = textureSize,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .sharingMode = vk::SharingMode::eExclusive,
    };
    mStagingBuffer = vk::raii::Buffer(context.getDevice(), stageBufferCreateInfo);

    const vk::MemoryRequirements memoryRequirements = mStagingBuffer.getMemoryRequirements();
    const vk::MemoryAllocateInfo memoryAllocateInfo {
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = vulkan_utils::findMemoryType(
            context.getPhysicalDevice(),
            memoryRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        )
    };
    mStagingBufferMemory = vk::raii::DeviceMemory(context.getDevice(), memoryAllocateInfo);
    mStagingBuffer.bindMemory(*mStagingBufferMemory, 0);

    void* data = mStagingBufferMemory.mapMemory(0, textureSize);
    std::memcpy(data, pixels, textureSize);
    mStagingBufferMemory.unmapMemory();

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
            imageMemRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eDeviceLocal
        )
    };
    mImageMemory = vk::raii::DeviceMemory(context.getDevice(), imageMemAllocateInfo);
    mImage.bindMemory(*mImageMemory, 0);

    const vk::ImageViewCreateInfo imageViewCreateInfo {
        .image = mImage,
        .viewType = vk::ImageViewType::e2D,
        .format = vk::Format::eR8G8B8A8Srgb,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    mImageView = vk::raii::ImageView(context.getDevice(), imageViewCreateInfo);
}

void StaticTexture::transitionLayout(const vk::raii::CommandBuffer& commandBuffer,
                                     vk::ImageLayout oldLayout,
                                     vk::ImageLayout newLayout) const {
    // Defining the barrier.
    vk::ImageMemoryBarrier barrier = {
        .oldLayout           = oldLayout,
        .newLayout           = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = mImage,
        .subresourceRange    = {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1}};
    // We need to tell the driver when and for what purpose in the pipeline
    // our resources need to be ready.
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;
    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage      = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument("Unsupported layout transition!");
    }
    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
}

void StaticTexture::prepare(const CommandDispatcher& dispatcher) const {
    // Creating a command buffer to prepare the texture.
    const vk::raii::CommandBuffer commandBuffer = dispatcher.getNewCommandBuffer();
    commandBuffer.begin({
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    });
    // Transition to a layout optimal for receiving transfer data.
    transitionLayout(commandBuffer, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    // Copying the whole buffer to the whole image.
    const vk::BufferImageCopy region {.bufferOffset      = 0,
                           .bufferRowLength   = 0,
                           .bufferImageHeight = 0,
                           .imageSubresource  = {.aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
                           .imageOffset       = {0, 0, 0},
                           .imageExtent       = {mWidth, mHeight, 1}};
    commandBuffer.copyBufferToImage(mStagingBuffer, mImage, vk::ImageLayout::eTransferDstOptimal, region);
    // Transition to a layout optimal for being read on fragment shaders.
    transitionLayout(commandBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    // Wrapping things up and submitting our command buffer.
    commandBuffer.end();
    dispatcher.getQueue().submit(vk::SubmitInfo {
        .commandBufferCount = 1,
        .pCommandBuffers = &*commandBuffer
    }, nullptr);
    dispatcher.getQueue().waitIdle();
}
