//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_COORDINATEGRID_HPP
#define OPENMAVIS_COORDINATEGRID_HPP

#include "AMORA/algebra_types.hpp"
#include "graphics/GraphicsPipeline.hpp"

namespace amora::scene {
    class CoordinateGrid {
    public:
        static constexpr uint32_t REQUIRED_DESCRIPTOR_SETS[] = {0};

        CoordinateGrid(const vk::raii::Device& device,
                       const vk::SurfaceFormatKHR& surfaceFormat,
                       const std::vector<vk::DescriptorSetLayout>& requiredLayouts);

        void draw(const vk::raii::CommandBuffer& commandBuffer, const DescriptorSetData& descriptorSetData) const;

    private:
        graphics::GraphicsPipeline mPipeline;

    };
}

#endif //OPENMAVIS_COORDINATEGRID_HPP
