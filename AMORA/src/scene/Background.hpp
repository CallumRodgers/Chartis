//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_BACKGROUND_HPP
#define CHARTIS_BACKGROUND_HPP

#include <vulkan/vulkan_raii.hpp>

#include "graphics/GraphicsPipeline.hpp"

namespace amora::scene {
    class Background {
    public:
        Background(const vk::raii::Device& device, const vk::SurfaceFormatKHR& surfaceFormat);

        void draw(const vk::raii::CommandBuffer& commandBuffer) const;

    private:
        graphics::GraphicsPipeline mPipeline;
    };
}

#endif //CHARTIS_BACKGROUND_HPP
