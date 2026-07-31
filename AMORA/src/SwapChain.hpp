//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_SWAPCHAIN_HPP
#define OPENMAVIS_SWAPCHAIN_HPP

#include "RenderSurface.hpp"

namespace amora {

    /**
     *
     */
    class SwapChain {
    public:
        SwapChain(const VulkanContext& context, const RenderSurface& renderSurface);
        ~SwapChain() { cleanUp(); }

        std::pair<bool, uint32_t> acquireNextImage(const vk::raii::Semaphore& renderCompleteSemaphore);

        [[nodiscard]] auto& get() const noexcept { return mSwapChain; }
        [[nodiscard]] auto& getSurfaceFormat() const noexcept { return mSwapChainSurfaceFormat; }
        [[nodiscard]] auto& getImages() const noexcept { return mSwapChainImages; }
        [[nodiscard]] auto& getImageViews() const noexcept { return mSwapChainImageViews; }
        [[nodiscard]] auto& getExtent() const noexcept { return mSwapChainExtent; }
        [[nodiscard]] auto getMinImageCount() const noexcept { return mMinImageCount; }

        void recreate();
        void cleanUp();

    private:
        const VulkanContext& mContext;
        const RenderSurface& mRenderSurface;

        vk::raii::SwapchainKHR mSwapChain = nullptr;
        std::vector<vk::Image> mSwapChainImages;
        std::vector<vk::raii::ImageView> mSwapChainImageViews;
        vk::SurfaceFormatKHR mSwapChainSurfaceFormat;
        vk::Extent2D mSwapChainExtent;
        uint32_t mMinImageCount = 0;

        void create(int width, int height);
    };
}

#endif //OPENMAVIS_SWAPCHAIN_HPP
