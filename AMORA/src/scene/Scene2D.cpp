//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include <AMORA/logger.hpp>
#include <AMORA/Scene2D.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "DescriptorAllocator.hpp"
#include "DescriptorLayoutManager.hpp"
#include "DescriptorSetData.hpp"
#include "UniformBuffer.hpp"
#include "graphics/GraphicsPipeline.hpp"

using namespace amora::scene;

Scene2D::Scene2D(const std::string &name) {
    mSceneName = name;
}

void Scene2D::setupCameraMatrices(const uint32_t surfaceWidth, const uint32_t surfaceHeight) {
    mCameraUBO.viewMatrix = glm::lookAt(m_vec3(0.f, 0.f, 1.f), m_vec3(0.f, 0.f, 0.f), m_vec3(0.f, 1.f, 0.f));
    updateSurfaceSize(surfaceWidth, surfaceHeight);
}

void Scene2D::updateSurfaceSize(uint32_t surfaceWidth, uint32_t surfaceHeight) {
    if (surfaceWidth > surfaceHeight) {
        const float aspectRatio = static_cast<m_float>(surfaceWidth) / static_cast<m_float>(surfaceHeight);
        mCameraUBO.projectionMatrix = glm::ortho(
            -PROJECTION_SCALE * aspectRatio, PROJECTION_SCALE * aspectRatio,
            -PROJECTION_SCALE, PROJECTION_SCALE,
            -1.f, 1.f
        );
    } else {
        const float aspectRatio = static_cast<m_float>(surfaceHeight) / static_cast<m_float>(surfaceWidth);
        mCameraUBO.projectionMatrix = glm::ortho(
            -PROJECTION_SCALE, PROJECTION_SCALE,
            -PROJECTION_SCALE * aspectRatio, PROJECTION_SCALE * aspectRatio,
            -1.f, 1.f
        );
    }

}

void Scene2D::setupVulkan(const VulkanContext& context, const SwapChain& swapChain, DescriptorLayoutManager& layoutManager) {

    const vk::raii::Device& device = context.getDevice();
    const std::vector requiredLayouts = {
        *layoutManager.fetchLayout(0)
    };

    mDescriptorAllocator = std::make_unique<DescriptorAllocator>(context);

    //////////////////////////
    // SETTING UP PIPELINES //
    //////////////////////////

    graphics::GraphicsPipeline::GraphicsPipelineCreateInfo backgroundPipelineCreateInfo {
        .shaderMap = {
            {Vertex, std::make_shared<Shader>(device, "background_vert")},
            {Fragment, std::make_shared<Shader>(device, "background_frag")}
        },
        .previousPipeline = nullptr,
        .surfaceFormat = swapChain.getSurfaceFormat()
    };
    mBackgroundPipeline = std::make_unique<graphics::GraphicsPipeline>(device, backgroundPipelineCreateInfo);

    graphics::GraphicsPipeline::GraphicsPipelineCreateInfo axesPipelineCreateInfo {
        .shaderMap = {
            {Vertex, std::make_shared<Shader>(device, "axis_vert")},
            {Fragment, std::make_shared<Shader>(device, "axis_frag")}
        },
        .previousPipeline = nullptr,
        .surfaceFormat = swapChain.getSurfaceFormat(),
        .requestedLayouts = requiredLayouts
    };
    mAxesPipeline = std::make_unique<graphics::GraphicsPipeline>(device, axesPipelineCreateInfo);

    //////////////////////////////////////
    // SETTING UP FRAME GROUP RESOURCES //
    //////////////////////////////////////

    for (uint32_t i = 0; i < config::rendering::MAX_FRAMES_IN_FLIGHT; i++) {
        mCameraBuffers[i] = std::make_unique<UniformBuffer<CameraBufferObject>>(context);
        mDescriptorSets[i] = std::make_unique<DescriptorSetData>(requiredLayouts, *mDescriptorAllocator);

        // Linking buffer to set
        auto bufferInfo = mCameraBuffers[i]->getDescriptorBufferInfo();
        mDescriptorSets[i]->linkToBuffer(bufferInfo, 0, 0);
    }
}

void Scene2D::recordScene(const vk::raii::CommandBuffer& commandBuffer, const uint32_t frameIndex) const {

    mCameraBuffers[frameIndex]->uploadData(mCameraUBO);

    const vk::raii::DescriptorSet& set = mDescriptorSets[frameIndex]->getDescriptorSets()[0];
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mAxesPipeline->getLayout(), 0, *set, nullptr);

    //////////////////////////
    // RECORDING BACKGROUND //
    //////////////////////////

    if (mBackgroundPipeline != nullptr) {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mBackgroundPipeline->get());
        commandBuffer.draw(3, 1, 0, 0);
    }

    /////////////////////////////
    // RECORDING AXES AND GRID //
    /////////////////////////////

    if (mAxesPipeline != nullptr) {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mAxesPipeline->get());
        commandBuffer.draw(3, 1, 0, 0);
    }

    for (auto pair : objects) {

    }
}
