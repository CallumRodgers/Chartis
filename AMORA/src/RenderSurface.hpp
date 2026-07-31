//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_RENDERSURFACE_HPP
#define OPENMAVIS_RENDERSURFACE_HPP

#include "VulkanContext.hpp"

// Forward declaring the window class to keep GLFW in the implementation file.
struct GLFWwindow;

namespace amora {

    /**
     * The RenderSurface class encapsulates the overall concept of a surface that may be drawn on.
     */
    class RenderSurface {
    public:
        RenderSurface(const VulkanContext& context, GLFWwindow* window);

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
        /** Retrieves the current framebuffer size of the window. */
        [[nodiscard]] std::pair<uint32_t, uint32_t> getCurrentFramebufferSize() const;
        /** Pauses the engine until the framebuffer size is valid (i.e., non-zero).
         * This is mostly useful for when the window was minimised, and we want to stop all
         * graphics work. */
        [[nodiscard]] std::pair<uint32_t, uint32_t> pauseUntilValidSize() const;

        [[nodiscard]] const vk::raii::SurfaceKHR& getSurface() const noexcept {
            return mSurface;
        }

        [[nodiscard]] GLFWwindow* getWindow() const noexcept {
            return mWindowHandle;
        }

    private:
        const VulkanContext& mContext;
        GLFWwindow* mWindowHandle;
        vk::raii::SurfaceKHR mSurface = nullptr;
    };
}

#endif //OPENMAVIS_RENDERSURFACE_HPP
