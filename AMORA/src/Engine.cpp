////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include <AMORA/Engine.hpp>
#include <AMORA/logger.hpp>
#include <AMORA/vector_utils.hpp>

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>

#include <backends/imgui_impl_vulkan.h>

#include "CommandDispatcher.hpp"
#include "DescriptorLayoutManager.hpp"
#include "FenceGroup.hpp"
#include "RenderSurface.hpp"
#include "SemaphoreGroup.hpp"
#include "SwapChain.hpp"
#include "TextureManager.hpp"
#include "VulkanContext.hpp"

using namespace amora;

void Engine::startLogging() {
    const auto sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("AMORA_console_sink");
    logger = quill::Frontend::create_or_get_logger("AMORA", sink);
    AMORA_INFO("Started AMORA logging");
}

void Engine::endLogging() {
    AMORA_INFO("Stopped AMORA logging");
    logger->flush_log();
}

Engine::Engine(const EngineInitInfo& initInfo) {
    ////////////////////////////////////
    // CREATING VULKAN MEMBER OBJECTS //
    ////////////////////////////////////

    AMORA_INFO("Begin AMORA Engine setup");

    // Creating the Vulkan context (instance, etc.)
    AMORA_INFO("Creating Vulkan context...");
    mContext = std::make_unique<VulkanContext>();

    // With the context set up, we create a surface from the window where we may render.
    AMORA_INFO("Creating rendering surface...");
    mRenderSurface = std::make_unique<RenderSurface>(*mContext, initInfo.platformInfo);
    mRenderSurface->invalidateSize(initInfo.surfaceWidth, initInfo.surfaceHeight);
    AMORA_INFO("Setting up physical and logical devices...");
    // Surface ready. We can set up the physical device (GPU) and the logical device
    // based on our application needs.
    mContext->setupDevices(mRenderSurface->getSurface());
    // Convenience for other constructors.
    const vk::raii::Device& device = mContext->getDevice();

    AMORA_INFO("Creating command dispatchers...");
    mGraphicsDispatcher = std::make_unique<CommandDispatcher>(device, mContext->getGraphicsIndex());
    mComputeDispatcher = std::make_unique<CommandDispatcher>(device, mContext->getComputeIndex());

    AMORA_INFO("Creating swap chain...");
    mSwapChain = std::make_unique<SwapChain>(*mContext, initInfo.surfaceWidth, initInfo.surfaceHeight, *mRenderSurface);

    AMORA_INFO("Initialising DescriptorLayoutManager...");
    mDescriptorManager = std::make_unique<DescriptorLayoutManager>(*mContext);

    AMORA_INFO("Initialising TextureManager...");
    mTextureManager = std::make_unique<TextureManager>(*mContext);

    AMORA_INFO("Creating synchronisation objects...");
    mPresentCompleteSemaphores = std::make_unique<SemaphoreGroup>(device, config::rendering::MAX_FRAMES_IN_FLIGHT);
    mRenderCompleteSemaphores = std::make_unique<SemaphoreGroup>(device, static_cast<uint32_t>(mSwapChain->getImages().size()));
    mDrawFences = std::make_unique<FenceGroup>(device, config::rendering::MAX_FRAMES_IN_FLIGHT);

    /////////////////////////////////////
    // CREATING DESCRIPTOR SET LAYOUTS //
    /////////////////////////////////////

    AMORA_INFO("Creating descriptor set layouts...");

    // Set 0 - Per-frame state: Camera matrices, etc.
    mDescriptorManager->addLayout({
        .setIndex = 0,
        .bindings = {
            // Camera UBO
            BindingInfo { .bindingIndex = 0, .bindingType = vk::DescriptorType::eUniformBuffer, .stageFlags = vk::ShaderStageFlagBits::eVertex }
        }
    });

    // Set 3 - Per-object state: Model matrices, etc.


    AMORA_INFO("AMORA Engine setup finished");
}

ImGuiEngineData Engine::beginInterfaceBackend() const {
    IMGUI_CHECKVERSION();
    auto* guiContext = ImGui::CreateContext();

    ImGui_ImplVulkan_InitInfo initInfo = {
        .ApiVersion = config::info::API_VERSION,
        .Instance = *mContext->getInstance(),
        .PhysicalDevice = *mContext->getPhysicalDevice(),
        .Device = *mContext->getDevice(),
        .QueueFamily = mGraphicsDispatcher->getFamilyIndex(),
        .Queue = *mGraphicsDispatcher->getQueue(),
        .DescriptorPoolSize = 100,
        .MinImageCount = mSwapChain->getMinImageCount(),
        .ImageCount = static_cast<uint32_t>(mSwapChain->getImages().size()),
        .PipelineCache = nullptr,
        .PipelineInfoMain = {
            .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
            .PipelineRenderingCreateInfo = vk::PipelineRenderingCreateInfo {
                .colorAttachmentCount = 1,
                .pColorAttachmentFormats = &mSwapChain->getSurfaceFormat().format
            }
        },
        .UseDynamicRendering = true
    };
    ImGui_ImplVulkan_Init(&initInfo);

    ImGuiEngineData data;
    data.Context = guiContext;
    ImGui::GetAllocatorFunctions(&data.AllocFunc, &data.FreeFunc, &data.AllocUserData);

    return data;
}

ImTextureID Engine::loadIcon(const resource_id& id, const char* iconPath) const {
    const StaticTexture& texture = mTextureManager->loadStaticTexture(*mGraphicsDispatcher, id, iconPath);
    return reinterpret_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(&**texture.getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
}

void Engine::endInterfaceBackend() const {
    ImGui_ImplVulkan_Shutdown();
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

void Engine::invalidateSurfaceSize(uint32_t width, uint32_t height) const {
    mRenderSurface->invalidateSize(width, height);
    mSwapChain->recreate();
    for (auto& scene : mLoadedScenes) {
        scene->updateSurfaceSize(width, height);
    }
}

void Engine::loadScene(scene::Scene& scene, const bool setActive) {
    auto [w, h] = mRenderSurface->getSurfaceSize();
    scene.setupCameraMatrices(w, h);
    scene.setupVulkan(*mContext, *mSwapChain, *mDescriptorManager);
    mLoadedScenes.push_back(&scene);
    if (setActive) {
        changeActiveScene(scene);
    }
}

void Engine::unloadScene(scene::Scene& scene) {
    if (vector_utils::contains(mLoadedScenes, &scene)) {
        mLoadedScenes.erase(mLoadedScenes.begin() + vector_utils::indexOf(mLoadedScenes, &scene));
    }
}

scene::Scene* Engine::changeActiveScene(scene::Scene& newScene) {
    if (std::ranges::find(mLoadedScenes, &newScene) == mLoadedScenes.end()) {
        throw std::invalid_argument("New scene must have been loaded before using!");
    }

    if (mActiveScene != nullptr) {
        scene::Scene* oldScene = mActiveScene;
        mActiveScene = &newScene;
        return oldScene;
    }
    mActiveScene = &newScene;
    return nullptr;
}

void Engine::beginFrame() {
    auto& device = mContext->getDevice();

    auto& drawFence = (*mDrawFences)[mFrameIndex];
    auto& presentCompleteSemaphore = (*mPresentCompleteSemaphores)[mFrameIndex];

    if (device.waitForFences(*drawFence, vk::True, UINT64_MAX) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fences!");
    }

    auto [successful, imageIndex] = mSwapChain->acquireNextImage(presentCompleteSemaphore);
    mImageIndex = imageIndex;

    if (!successful) {
        mSwapChain->recreate();
        mFrameIndex = (mFrameIndex + 1) % config::rendering::MAX_FRAMES_IN_FLIGHT;
        return;
    }

    device.resetFences(*drawFence);

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

    commandBuffer.setViewport(0, vk::Viewport(
        0.0f, 0.0f,
        static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height),
        0.0f, 1.0f
    ));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));

    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
}

void Engine::endFrame() {
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();

    const vk::raii::CommandBuffer& commandBuffer = mGraphicsDispatcher->getCommandBuffer(mFrameIndex);
    auto& drawFence = (*mDrawFences)[mFrameIndex];
    auto& presentCompleteSemaphore = (*mPresentCompleteSemaphores)[mFrameIndex];

    mActiveScene->recordScene(mGraphicsDispatcher->getCommandBuffer(mFrameIndex), mFrameIndex);

    ImGui_ImplVulkan_RenderDrawData(drawData, *commandBuffer);

    commandBuffer.endRendering();

    transitionImageLayout(
        *mSwapChain,
        *mGraphicsDispatcher,
        mFrameIndex,
        mImageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
        {},                                                     // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
        vk::PipelineStageFlagBits2::eBottomOfPipe               // dstStage
    );

    commandBuffer.end();

    auto& renderCompleteSemaphore = (*mRenderCompleteSemaphores)[mImageIndex];

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
        .pImageIndices      = &mImageIndex
    };

    if (const vk::Result result = mGraphicsDispatcher->getQueue().presentKHR(presentInfo);
        result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
    {
        mSwapChain->recreate();
    }

    mFrameIndex = (mFrameIndex + 1) % config::rendering::MAX_FRAMES_IN_FLIGHT;
}

void Engine::waitIdle() const {
    mContext->getDevice().waitIdle();
}

Engine::~Engine() {
    terminate();
    endLogging();
}

void Engine::terminate() const {
    mContext->getDevice().waitIdle();
    mSwapChain->cleanUp();
}
