////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsPipeline.hpp"

#include <ranges>
#include <AMORA/vector_utils.hpp>

using namespace amora::graphics;

GraphicsPipeline::GraphicsPipeline(const vk::raii::Device& device,
                                   const GraphicsPipelineCreateInfo& createInfo)
    : Pipeline(device, createInfo.requestedLayouts, createInfo.shaderMap)
{
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

    mRenderingCreateInfo = {
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &(createInfo.surfaceFormat.format)
    };

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
        {
            .stageCount          = static_cast<uint32_t>(mShaderStages.size()),
            .pStages             = mShaderStages.data(),
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
        mRenderingCreateInfo
    };

    if (createInfo.previousPipeline != nullptr) {
        const vk::raii::PipelineCache& cache = createInfo.previousPipeline->getPipelineCache();
        mPipeline = vk::raii::Pipeline(
            device, cache, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
        );
    } else {
        mPipeline = vk::raii::Pipeline(
            device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
        );
    }
}
