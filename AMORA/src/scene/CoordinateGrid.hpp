////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_COORDINATEGRID_HPP
#define CHARTIS_COORDINATEGRID_HPP

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

#endif //CHARTIS_COORDINATEGRID_HPP
