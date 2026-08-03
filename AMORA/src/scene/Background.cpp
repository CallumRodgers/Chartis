//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "Background.hpp"

using namespace amora::scene;

Background::Background(const vk::raii::Device& device, const vk::SurfaceFormatKHR& surfaceFormat)
    : mPipeline(device, {
        .shaderMap = {
                {Vertex, std::make_shared<Shader>(device, "background_vert")},
                {Fragment, std::make_shared<Shader>(device, "background_frag")}
            },
        .previousPipeline = nullptr,
        .surfaceFormat = surfaceFormat
    }) {}

void Background::draw(const vk::raii::CommandBuffer& commandBuffer) const {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline.get());
    commandBuffer.draw(3, 1, 0, 0);
}
