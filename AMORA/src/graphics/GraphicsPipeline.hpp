//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_GRAPHICSPIPELINE_HPP
#define OPENMAVIS_GRAPHICSPIPELINE_HPP

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "SwapChain.hpp"

namespace amora::graphics {
    /**
     *
     */
    class GraphicsPipeline final : public Pipeline {
    public:
        struct GraphicsPipelineCreateInfo {
            const ShaderMap& shaderMap;
            const Pipeline* previousPipeline;
            const vk::SurfaceFormatKHR& surfaceFormat;
            const std::vector<vk::DescriptorSetLayout> requestedLayouts;
        };

        GraphicsPipeline(const vk::raii::Device& device, const GraphicsPipelineCreateInfo& createInfo);

        [[nodiscard]] const vk::PipelineRenderingCreateInfo& getRenderingCreateInfo() const {
            return mRenderingCreateInfo;
        }

    private:
        vk::PipelineRenderingCreateInfo mRenderingCreateInfo;


    };
}

#endif //OPENMAVIS_GRAPHICSPIPELINE_HPP
