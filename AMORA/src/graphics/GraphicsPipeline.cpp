////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include "GraphicsPipeline.hpp"

using namespace AMORA::Graphics;

GraphicsPipeline::GraphicsPipeline(
    const vk::raii::Device& device, const std::map<ShaderStage, std::shared_ptr<Shader> >& shaders,
    const Pipeline* previousPipeline, const SwapChain& swapChain
    ) : Pipeline(shaders)
{
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = createStages(shaders);

    // =======================
    // Creating PipelineLayout
    // =======================

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };
    mPipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

    // ====================================================
    // Creating Pipeline fixed functions and configurations
    // ====================================================

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

    std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo {
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };
    vk::PipelineViewportStateCreateInfo viewportStateInfo {
        .viewportCount = 1, .scissorCount = 1
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo {
        .topology = vk::PrimitiveTopology::eTriangleList
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer {
        .depthClampEnable        = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode             = vk::PolygonMode::eFill,
        .cullMode                = vk::CullModeFlagBits::eBack,
        .frontFace               = vk::FrontFace::eClockwise,
        .depthBiasEnable         = vk::False,
        .lineWidth               = 1.0f
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment {
        .blendEnable         = vk::True,
        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .colorBlendOp        = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp        = vk::BlendOp::eAdd,
        .colorWriteMask      = vk::ColorComponentFlags(
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
    };

    vk::PipelineColorBlendStateCreateInfo colorBlending {
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    vk::PipelineMultisampleStateCreateInfo multisampleInfo {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False
    };

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
        {
            .stageCount          = static_cast<uint32_t>(shaderStages.size()),
            .pStages             = shaderStages.data(),
            .pVertexInputState   = &vertexInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pViewportState      = &viewportStateInfo,
            .pRasterizationState = &rasterizer,
            .pMultisampleState   = &multisampleInfo,
            .pColorBlendState    = &colorBlending,
            .pDynamicState       = &dynamicStateInfo,
            .layout              = mPipelineLayout,
            .renderPass          = nullptr // Using dynamic rendering.
        },
        {
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &swapChain.getSurfaceFormat().format
        }
    };

    if (previousPipeline != nullptr) {
        const vk::raii::PipelineCache& cache = previousPipeline->getPipelineCache();
        mPipeline = vk::raii::Pipeline(
            device, cache, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
        );
    } else {
        mPipeline = vk::raii::Pipeline(
            device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
        );
    }
}

std::vector<vk::PipelineShaderStageCreateInfo> GraphicsPipeline::createStages(const std::map<ShaderStage, std::shared_ptr<Shader> > &shaders) {
    // std::map.at() will throw an error if either the vertex or the
    // fragment shaders aren't specified. The tesselation shader is optional.
    const std::shared_ptr<Shader> vertexShader = shaders.at(VERTEX);
    const std::shared_ptr<Shader> fragmentShader = shaders.at(FRAGMENT);

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

    if (shaders.contains(TESSELATION)) {
        const std::shared_ptr<Shader> tesselationShader = shaders.at(TESSELATION);

        vk::PipelineShaderStageCreateInfo tesselationControlStageInfo {
            .stage = vk::ShaderStageFlagBits::eTessellationControl,
            .module = tesselationShader->getModule(),
            .pName = SHADER_ENTRY_POINT
        };

        vk::PipelineShaderStageCreateInfo tesselationEvaluationStageInfo {
            .stage = vk::ShaderStageFlagBits::eTessellationEvaluation,
            .module = tesselationShader->getModule(),
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
