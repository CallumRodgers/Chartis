////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

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
        GraphicsPipeline(
            const vk::raii::Device& device, const std::map<ShaderStage, std::shared_ptr<Shader> >& shaders,
            const Pipeline* previousPipeline, const SwapChain& swapChain
            );

        [[nodiscard]] const vk::PipelineRenderingCreateInfo& getRenderingCreateInfo() const {
            return mRenderingCreateInfo;
        }

    private:
        vk::PipelineRenderingCreateInfo mRenderingCreateInfo;

        [[nodiscard]] std::vector<vk::PipelineShaderStageCreateInfo> createStages(const std::map<ShaderStage, std::shared_ptr<Shader> >& shaders);
    };
}

#endif //OPENMAVIS_GRAPHICSPIPELINE_HPP
