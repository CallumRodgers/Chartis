//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "CoordinateGrid.hpp"

#include "DescriptorSetData.hpp"

using namespace amora::scene;

CoordinateGrid::CoordinateGrid(const vk::raii::Device& device,
                               const vk::SurfaceFormatKHR& surfaceFormat,
                               const std::vector<vk::DescriptorSetLayout>& requiredLayouts)
        : mPipeline(device, {
            .shaderMap = {
                {Vertex, std::make_shared<Shader>(device, "scene2D/coordinate_grid_2D_vert")},
                {Fragment, std::make_shared<Shader>(device, "scene2D/coordinate_grid_2D_cartesian")}
            },
            .previousPipeline = nullptr,
            .surfaceFormat = surfaceFormat,
            .requestedLayouts = requiredLayouts
        }) {}

void CoordinateGrid::draw(const vk::raii::CommandBuffer& commandBuffer, const DescriptorSetData& descriptorSetData) const {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline.get());
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                  mPipeline.getLayout(),
                                  0,
                                  *descriptorSetData.getDescriptorSets()[0],
                                  nullptr);

    commandBuffer.draw(3, 1, 0, 0);
}
