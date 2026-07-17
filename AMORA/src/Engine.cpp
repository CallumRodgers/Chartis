////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include <AMORA/Engine.hpp>
#include <AMORA/logger.hpp>

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>

#include "VulkanContext.hpp"
#include "RenderSurface.hpp"
#include "CommandDispatcher.hpp"
#include "SwapChain.hpp"
#include "Shader.hpp"
#include "graphics/GraphicsPipeline.hpp"
#include "SemaphoreGroup.hpp"
#include "FenceGroup.hpp"

using namespace amora;

void Engine::startLogging() {
    quill::Backend::start();
    const auto sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("AMORA_console_sink");
    logger = quill::Frontend::create_or_get_logger("AMORA", sink);
    ENGINE_INFO("Successfully initialized AMORA logger.");
}

void Engine::endLogging() {
    ENGINE_INFO("Stopping AMORA log.");
    logger->flush_log();
    quill::Backend::stop();
}

Engine::Engine(GLFWwindow* window) {
    // Creating the Vulkan context (instance, etc.)
    mContext = std::make_unique<VulkanContext>();

    mRenderSurface = std::make_unique<RenderSurface>(*mContext, window);

    mContext->setupDevices(mRenderSurface->getSurface());

    const vk::raii::Device& device = mContext->getDevice();

    mGraphicsDispatcher = std::make_unique<CommandDispatcher>(device, mContext->getGraphicsIndex());
    mComputeDispatcher = std::make_unique<CommandDispatcher>(device, mContext->getComputeIndex());

    mSwapChain = std::make_unique<SwapChain>(*mContext, *mRenderSurface);

    mPresentCompleteSemaphores = std::make_unique<SemaphoreGroup>(device, config::rendering::MAX_FRAMES_IN_FLIGHT);
    mRenderCompleteSemaphores = std::make_unique<SemaphoreGroup>(device, static_cast<uint32_t>(mSwapChain->getImages().size()));
    mDrawFences = std::make_unique<FenceGroup>(device, config::rendering::MAX_FRAMES_IN_FLIGHT);

    std::map<ShaderStage, std::shared_ptr<Shader> > shaders {
        {VERTEX,    std::make_shared<Shader>(device, "shader_vert")},
        {FRAGMENT,  std::make_shared<Shader>(device, "shader_frag")}
    };

    mPipeline = std::make_unique<graphics::GraphicsPipeline>(device, shaders, nullptr, *mSwapChain);
}

ImGuiEngineData Engine::beginUserInterface(GLFWwindow *window) const {
    IMGUI_CHECKVERSION();
    auto* guiContext = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    const float scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    style.FontSizeBase = 13.0f;
    ImFont* robotoFont = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf");
    ImGui::PushFont(robotoFont);

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo initInfo = {
        .ApiVersion = config::info::API_VERSION,
        .Instance = *mContext->getInstance(),
        .PhysicalDevice = *mContext->getPhysicalDevice(),
        .Device = *mContext->getDevice(),
        .QueueFamily = mGraphicsDispatcher->getFamilyIndex(),
        .Queue = *mGraphicsDispatcher->getQueue(),
        .DescriptorPoolSize = IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE,
        .MinImageCount = mSwapChain->getMinImageCount(),
        .ImageCount = static_cast<uint32_t>(mSwapChain->getImages().size()),
        .PipelineCache = nullptr,
        .PipelineInfoMain = {
            .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
            .PipelineRenderingCreateInfo = mPipeline->getRenderingCreateInfo()
        },
        .UseDynamicRendering = true
    };
    ImGui_ImplVulkan_Init(&initInfo);

    ImGuiEngineData data;
    data.Context = guiContext;
    ImGui::GetAllocatorFunctions(&data.AllocFunc, &data.FreeFunc, &data.AllocUserData);

    return data;
}

void Engine::endUserInterface() const {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void transitionImageLayout(
        const SwapChain& swapChain,
        const CommandDispatcher& dispatcher,
        uint32_t                frameIndex,
        uint32_t                imageIndex,
        vk::ImageLayout         old_layout,
        vk::ImageLayout         new_layout,
        vk::AccessFlags2        src_access_mask,
        vk::AccessFlags2        dst_access_mask,
        vk::PipelineStageFlags2 src_stage_mask,
        vk::PipelineStageFlags2 dst_stage_mask) {
    vk::ImageMemoryBarrier2 barrier = {
        .srcStageMask        = src_stage_mask,
        .srcAccessMask       = src_access_mask,
        .dstStageMask        = dst_stage_mask,
        .dstAccessMask       = dst_access_mask,
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = swapChain.getImages()[imageIndex],
        .subresourceRange    = {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1}};
    vk::DependencyInfo dependency_info = {
        .dependencyFlags         = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &barrier};
    dispatcher.getCommandBuffer(frameIndex).pipelineBarrier2(dependency_info);
}

void Engine::recordCommandBuffer(uint32_t frameIndex, uint32_t imageIndex) const {

    const vk::raii::CommandBuffer& commandBuffer = mGraphicsDispatcher->getCommandBuffer(mFrameIndex);
    commandBuffer.reset();
    commandBuffer.begin({});

    transitionImageLayout(
        *mSwapChain,
        *mGraphicsDispatcher,
        mFrameIndex,
        imageIndex,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput
    );

    constexpr vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo {
        .imageView   = mSwapChain->getImageViews()[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eStore,
        .clearValue  = clearColor
    };

    const vk::Extent2D swapChainExtent = mSwapChain->getExtent();

    const vk::RenderingInfo renderInfo {
        .renderArea = {.offset = {0,0}, .extent = swapChainExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo
    };

    commandBuffer.beginRendering(renderInfo);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline->get());
    commandBuffer.setViewport(0, vk::Viewport(
        0.0f, 0.0f,
        static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height),
        0.0f, 1.0f
    ));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));

    commandBuffer.draw(3, 1, 0, 0);

    ImGui_ImplVulkan_RenderDrawData(drawData, *commandBuffer);

    commandBuffer.endRendering();

    transitionImageLayout(
        *mSwapChain,
        *mGraphicsDispatcher,
        mFrameIndex,
        imageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
        {},                                                     // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
        vk::PipelineStageFlagBits2::eBottomOfPipe               // dstStage
    );

    commandBuffer.end();
}

void Engine::renderActive() {
    auto& device = mContext->getDevice();

    auto& drawFence = (*mDrawFences)[mFrameIndex];
    auto& presentCompleteSemaphore = (*mPresentCompleteSemaphores)[mFrameIndex];

    if (device.waitForFences(*drawFence, vk::True, UINT64_MAX) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fences!");
    }

    auto [successful, imageIndex] = mSwapChain->acquireNextImage(presentCompleteSemaphore);

    if (!successful) {
        mSwapChain->recreate();
        mFrameIndex = (mFrameIndex + 1) % config::rendering::MAX_FRAMES_IN_FLIGHT;
        return;
    }

    device.resetFences(*drawFence);

    const vk::raii::CommandBuffer& commandBuffer = mGraphicsDispatcher->getCommandBuffer(mFrameIndex);

    recordCommandBuffer(mFrameIndex, imageIndex);

    auto& renderCompleteSemaphore = (*mRenderCompleteSemaphores)[imageIndex];

    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo submitInfo {
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &*presentCompleteSemaphore,
        .pWaitDstStageMask    = &waitDestinationStageMask,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &*commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &*renderCompleteSemaphore
    };

    mGraphicsDispatcher->getQueue().submit(submitInfo, *drawFence);

    const vk::PresentInfoKHR presentInfo {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &*renderCompleteSemaphore,
        .swapchainCount     = 1,
        .pSwapchains        = &*mSwapChain->get(),
        .pImageIndices      = &imageIndex
    };

    if (const vk::Result result = mGraphicsDispatcher->getQueue().presentKHR(presentInfo);
        result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR || mSwapChainInvalidated)
        {
        mSwapChainInvalidated = false;
        mSwapChain->recreate();
    }

    mFrameIndex = (mFrameIndex + 1) % config::rendering::MAX_FRAMES_IN_FLIGHT;
}

Engine::~Engine() {
    terminate();
    endLogging();
}

void Engine::terminate() const {
    mContext->getDevice().waitIdle();
    mSwapChain->cleanUp();
}
