////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include "RenderSurface.hpp"

#include <GLFW/glfw3.h>

using namespace AMORA;

RenderSurface::RenderSurface(const VulkanContext& context, GLFWwindow* window)
    : mContext(context), mWindowHandle(window)
{
    const vk::raii::Instance& instance = context.getInstance();
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(*instance, mWindowHandle, nullptr, &_surface) != 0) {
        throw std::runtime_error("Failed to create window surface.");
    }
    mSurface = vk::raii::SurfaceKHR(instance, _surface);
}

std::pair<int, int> RenderSurface::getCurrentFramebufferSize() const {
    int w, h;
    glfwGetFramebufferSize(mWindowHandle, &w, &h);
    return {w, h};
}

std::pair<int, int> RenderSurface::pauseUntilValidSize() const {
    auto [w, h] = getCurrentFramebufferSize();
    while (w == 0 || h == 0) {
        glfwWaitEvents();
        glfwGetFramebufferSize(mWindowHandle, &w, &h);
    }
    return {w, h};
}
