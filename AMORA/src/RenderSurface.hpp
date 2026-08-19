//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_RENDERSURFACE_HPP
#define OPENMAVIS_RENDERSURFACE_HPP

#include "VulkanContext.hpp"
#include "AMORA/Platform.hpp"

namespace amora {

    /**
     * The RenderSurface class encapsulates the overall concept of a surface that may be drawn on.
     */
    class RenderSurface {
    public:
        RenderSurface(const VulkanContext& context, const NativePlatformInfo& platformInfo);

        void invalidateSize(uint32_t width, uint32_t height) {
            if (width <= 0 || height <= 0) throw std::invalid_argument("Surface dimensions must be positive.");
            mSurfaceWidth = width;
            mSurfaceHeight = height;
        }

        /** Retrieves the capabilities of the current surface from the physical device. */
        [[nodiscard]] auto getCurrentCapabilities() const noexcept {
            return mContext.getPhysicalDevice().getSurfaceCapabilitiesKHR(*mSurface);
        }
        /** Retrieves the available formats of the current surface from the physical device. */
        [[nodiscard]] auto getCurrentFormats() const noexcept {
            return mContext.getPhysicalDevice().getSurfaceFormatsKHR(*mSurface);
        }
        /** Retrieves the available present modes of the current surface from the physical device. */
        [[nodiscard]] std::vector<vk::PresentModeKHR> getCurrentPresentModes() const noexcept {
            return mContext.getPhysicalDevice().getSurfacePresentModesKHR(*mSurface);
        }
        /** Retrieves the current size of the surface, which is the same as the window */
        [[nodiscard]] std::pair<uint32_t, uint32_t> getSurfaceSize() const noexcept {
            return {mSurfaceWidth, mSurfaceHeight};
        }

        [[nodiscard]] const vk::raii::SurfaceKHR& getSurface() const noexcept {
            return mSurface;
        }

    private:
        const VulkanContext& mContext;
        vk::raii::SurfaceKHR mSurface = nullptr;

        uint32_t mSurfaceWidth;
        uint32_t mSurfaceHeight;
    };
}

#endif //OPENMAVIS_RENDERSURFACE_HPP
