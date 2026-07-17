////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include "SwapChain.hpp"

using namespace amora;

SwapChain::SwapChain(const VulkanContext& context, const RenderSurface& renderSurface)
    : mContext(context), mRenderSurface(renderSurface)
{
    auto [width, height] = renderSurface.getCurrentFramebufferSize();
    create(width, height);
}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats) {
    const auto formatIt = std::ranges::find_if(
            availableFormats,
            [](const auto &format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes) {
    bool supportsFifo = false;
    bool supportsFifoRelaxed = false;

    // Checking for FIFO and Mailbox modes.
    std::ranges::for_each(availablePresentModes, [&supportsFifoRelaxed, &supportsFifo](auto presentMode) {
        if (presentMode == vk::PresentModeKHR::eFifoRelaxed) {
            supportsFifoRelaxed = true;
        } else if (presentMode == vk::PresentModeKHR::eFifo) {
            supportsFifo = true;
        }
    });

    // All graphics cards should support FIFO mode.
    assert(supportsFifo);
    return supportsFifoRelaxed ? vk::PresentModeKHR::eFifoRelaxed : vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapExtent(const int width, const int height, vk::SurfaceCapabilitiesKHR const &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount)) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
}

void SwapChain::create(const int width, const int height) {
    const auto surfaceCapabilities = mRenderSurface.getCurrentCapabilities();
    mSwapChainExtent = chooseSwapExtent(width, height, surfaceCapabilities);
    mMinImageCount = chooseSwapMinImageCount(surfaceCapabilities);

    const auto availableFormats = mRenderSurface.getCurrentFormats();
    mSwapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    const auto presentMode = chooseSwapPresentMode(mRenderSurface.getCurrentPresentModes());

    const vk::SwapchainCreateInfoKHR swapChainCreateInfo = {
                    .surface          = *mRenderSurface.getSurface(),
                    .minImageCount    = mMinImageCount,
                    .imageFormat      = mSwapChainSurfaceFormat.format,
                    .imageColorSpace  = mSwapChainSurfaceFormat.colorSpace,
                    .imageExtent      = mSwapChainExtent,
                    .imageArrayLayers = 1,
                    .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                    .imageSharingMode = vk::SharingMode::eExclusive,
                    .preTransform     = surfaceCapabilities.currentTransform,
                    .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                    .presentMode      = presentMode,
                    .clipped          = true,
                    .oldSwapchain     = nullptr
    };

    mSwapChain = vk::raii::SwapchainKHR(mContext.getDevice(), swapChainCreateInfo);
    mSwapChainImages = mSwapChain.getImages();

    // ====================================
    // Creating ImageViews
    // ====================================

    assert(mSwapChainImageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo = {
        .viewType = vk::ImageViewType::e2D,
        .format = mSwapChainSurfaceFormat.format,
        .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor, .levelCount = 1, .layerCount = 1}
    };

    for (const auto &image : mSwapChainImages) {
        imageViewCreateInfo.image = image;
        mSwapChainImageViews.emplace_back(mContext.getDevice(), imageViewCreateInfo);
    }
}

std::pair<bool, uint32_t> SwapChain::acquireNextImage(const vk::raii::Semaphore& renderCompleteSemaphore) {
    auto [result, imageIndex] = mSwapChain.acquireNextImage(UINT64_MAX, renderCompleteSemaphore, nullptr);
    if (result == vk::Result::eErrorOutOfDateKHR) {
        return {false, imageIndex};
    }
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
        throw std::runtime_error("Failed to acquire swap chain image!");
    }
    return {true, imageIndex};
}

void SwapChain::recreate() {
    // If the app is minimised, it'll pause until we get a valid size again.
    auto [width, height] = mRenderSurface.pauseUntilValidSize();
    mContext.getDevice().waitIdle();
    cleanUp();
    create(width, height);
}

void SwapChain::cleanUp() {
    mSwapChainImageViews.clear();
    mSwapChain = nullptr;
}
