////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_PIPELINE_HPP
#define CHARTIS_PIPELINE_HPP

#include <map>

#include "DescriptorLayoutManager.hpp"
#include "Shader.hpp"

namespace amora {
    /**
     * The <code>Pipeline</code> class encapsulates the general properties of a pipeline object,
     * whether graphics or compute. Many functions are declared virtual to be implemented in either case.
     */
    class Pipeline {
    public:
        Pipeline() = delete;
        virtual ~Pipeline() = default;

        [[nodiscard]] auto& get() const { return mPipeline; }
        [[nodiscard]] auto& getLayout() const noexcept { return mPipelineLayout; }
        [[nodiscard]] auto& getPipelineCache() const { return mPipelineCache; }

    protected:
        Pipeline(const vk::raii::Device& device,
                 const std::vector<vk::DescriptorSetLayout>& requiredLayouts,
                 const ShaderMap& shaders);

        [[nodiscard]] static std::vector<vk::PipelineShaderStageCreateInfo> createStages(const ShaderMap& shaders);

        ShaderMap mShaders;
        std::vector<vk::PipelineShaderStageCreateInfo> mShaderStages;
        vk::raii::PipelineLayout mPipelineLayout {nullptr};
        vk::raii::Pipeline mPipeline {nullptr};
        vk::raii::PipelineCache mPipelineCache {nullptr};
    };
}

#endif //CHARTIS_PIPELINE_HPP
