////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include "Pipeline.hpp"
#include "AMORA/logger.hpp"

using namespace amora;

Pipeline::Pipeline(const vk::raii::Device& device,
                   const std::vector<vk::DescriptorSetLayout>& requiredLayouts,
                   const ShaderMap& shaders) {
    mShaderStages = createStages(shaders);

    //////////////////////////////
    // CREATING PIPELINE LAYOUT //
    //////////////////////////////

    const vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
        .setLayoutCount = static_cast<uint32_t>(requiredLayouts.size()),
        .pSetLayouts = requiredLayouts.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };
    mPipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);
}

std::vector<vk::PipelineShaderStageCreateInfo> Pipeline::createStages(const ShaderMap& shaders) {
    // If this is a compute pipeline, only the compute shader matters.
    if (shaders.contains(Compute)) {
        if (shaders.size() > 1) {
            AMORA_WARN("Compute pipeline declared with more than one shader. Ignoring others.");
        }

        const std::shared_ptr<Shader> computeShader = shaders.at(Compute);

        vk::PipelineShaderStageCreateInfo computeCreateInfo {
            .stage = vk::ShaderStageFlagBits::eCompute,
            .module = computeShader->getModule(),
            .pName = SHADER_ENTRY_POINT
        };

        return {computeCreateInfo};
    }

    // If we're here, this is a graphics pipeline.

    // std::map.at() will throw an error if either the vertex or the
    // fragment shaders aren't specified. The tesselation shader is optional.
    const std::shared_ptr<Shader> vertexShader = shaders.at(Vertex);
    const std::shared_ptr<Shader> fragmentShader = shaders.at(Fragment);

    // Creating vertex shader info.
    vk::PipelineShaderStageCreateInfo vertexStageInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = vertexShader->getModule(),
        .pName = SHADER_ENTRY_POINT
    };

    vk::PipelineShaderStageCreateInfo fragmentStageInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = fragmentShader->getModule(),
        .pName = SHADER_ENTRY_POINT
    };

    if (shaders.contains(TesselationControl)) {
        const std::shared_ptr<Shader> tesselationControlShader = shaders.at(TesselationControl);
        // Must be present if CONTROL is present.
        const std::shared_ptr<Shader> tesselationEvaluationShader = shaders.at(TesselationEvaluation);

        vk::PipelineShaderStageCreateInfo tesselationControlStageInfo {
            .stage = vk::ShaderStageFlagBits::eTessellationControl,
            .module = tesselationControlShader->getModule(),
            .pName = SHADER_ENTRY_POINT
        };

        vk::PipelineShaderStageCreateInfo tesselationEvaluationStageInfo {
            .stage = vk::ShaderStageFlagBits::eTessellationEvaluation,
            .module = tesselationEvaluationShader->getModule(),
            .pName = SHADER_ENTRY_POINT
        };

        return {
            vertexStageInfo,
            tesselationControlStageInfo,
            tesselationEvaluationStageInfo,
            fragmentStageInfo
        };
    }
    return {
        vertexStageInfo,
        fragmentStageInfo
    };
}
