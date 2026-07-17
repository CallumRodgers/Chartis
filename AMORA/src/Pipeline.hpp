////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_PIPELINE_HPP
#define OPENMAVIS_PIPELINE_HPP

#include "Shader.hpp"
#include <map>

namespace amora {
    /**
     *
     */
    class Pipeline {
    public:
        Pipeline() = delete;
        virtual ~Pipeline() = default;

        [[nodiscard]] auto& get() const { return mPipeline; }
        [[nodiscard]] auto& getPipelineCache() const { return mPipelineCache; }

    protected:
        explicit Pipeline(const std::map<ShaderStage, std::shared_ptr<Shader> >& shaders) : mShaders(shaders) {}

        std::map<ShaderStage, std::shared_ptr<Shader> > mShaders;

        vk::raii::PipelineLayout mPipelineLayout = nullptr;
        vk::raii::Pipeline mPipeline = nullptr;
        vk::raii::PipelineCache mPipelineCache = nullptr;
    };
}

#endif //OPENMAVIS_PIPELINE_HPP
