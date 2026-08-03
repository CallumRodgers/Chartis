//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include <AMORA/logger.hpp>
#include <AMORA/Scene2D.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "Background.hpp"
#include "CoordinateGrid.hpp"
#include "DescriptorAllocator.hpp"
#include "DescriptorLayoutManager.hpp"
#include "DescriptorSetData.hpp"
#include "UniformBuffer.hpp"
#include "AMORA/vector_utils.hpp"
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
    invalidateCameraMatrices();
}

void Scene2D::setupVulkan(const VulkanContext& context, const SwapChain& swapChain, DescriptorLayoutManager& layoutManager) {

    const vk::raii::Device& device = context.getDevice();

    mDescriptorAllocator = std::make_unique<DescriptorAllocator>(context);
    std::unordered_map<uint32_t, vk::DescriptorSetLayout> totalRequiredLayouts;

    //////////////////////////////
    // SETTING UP SCENE OBJECTS //
    //////////////////////////////

    mBackground = std::make_unique<Background>(device, swapChain.getSurfaceFormat());

    std::vector<vk::DescriptorSetLayout> requiredCoordinateGridLayouts;
    for (const uint32_t setIndex : CoordinateGrid::REQUIRED_DESCRIPTOR_SETS) {
        vk::DescriptorSetLayout layout = *layoutManager.fetchLayout(setIndex);

        requiredCoordinateGridLayouts.push_back(layout);
        if (!totalRequiredLayouts.contains(setIndex)) {
            totalRequiredLayouts.emplace(setIndex, layout);
        }
    }
    mCoordinateGrid = std::make_unique<CoordinateGrid>(device, swapChain.getSurfaceFormat(), requiredCoordinateGridLayouts);

    //////////////////////////////////////
    // SETTING UP FRAME GROUP RESOURCES //
    //////////////////////////////////////

    for (uint32_t i = 0; i < config::rendering::MAX_FRAMES_IN_FLIGHT; i++) {
        mCameraBuffers[i] = std::make_unique<UniformBuffer<CameraUBO>>(context);
        mDescriptorSets[i] = std::make_unique<DescriptorSetData>(vector_utils::fromUnorderedMapValues(totalRequiredLayouts), *mDescriptorAllocator);

        // Linking buffer to set
        auto bufferInfo = mCameraBuffers[i]->getDescriptorBufferInfo();
        mDescriptorSets[i]->linkToBuffer(bufferInfo, 0, 0);
    }
}

void Scene2D::recordScene(const vk::raii::CommandBuffer& commandBuffer, const uint32_t frameIndex) const {
    ///////////////////////
    // SCENE PREPARATION //
    ///////////////////////

    mCameraBuffers[frameIndex]->uploadData(mCameraUBO);

    ///////////////////
    // SCENE DRAWING //
    ///////////////////

    // Recording background.
    if (mBackground != nullptr) { mBackground->draw(commandBuffer); }
    // Recording axes and grid.
    if (mCoordinateGrid != nullptr) { mCoordinateGrid->draw(commandBuffer, *mDescriptorSets[frameIndex]); }

    for (auto pair : objects) {

    }
}

void Scene2D::invalidateCameraMatrices() {
    mCameraUBO.inverseMatrix = glm::inverse(mCameraUBO.projectionMatrix * mCameraUBO.viewMatrix);
}
