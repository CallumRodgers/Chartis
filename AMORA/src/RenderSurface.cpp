//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "RenderSurface.hpp"

using namespace amora;

using chartis::platypus::NativePlatformInfo;
using chartis::platypus::Platform;

//////////////////////////////////
// BEGIN PLATFORM-SPECIFIC CODE //
//////////////////////////////////
#if defined(__linux__) || defined(__FreeBSD__)
///////////////////////////////
// BEGIN LINUX PLATFORM CODE //
#include <wayland-client.h>
#include <xcb/xcb.h>

#include <vulkan/vulkan_wayland.h>
#include <vulkan/vulkan_xcb.h>

void createSurface(const NativePlatformInfo& platformInfo, const vk::raii::Instance& instance, VkSurfaceKHR* surface) {
    switch (platformInfo.platform) {
        default: { throw std::runtime_error("Platform mismatch."); }
        case Platform::Wayland: {
            const VkWaylandSurfaceCreateInfoKHR createInfo {
                .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
                .display = static_cast<wl_display*>(platformInfo.displayHandle),
                .surface = static_cast<wl_surface*>(platformInfo.windowHandle)
            };
            vkCreateWaylandSurfaceKHR(*instance, &createInfo, nullptr, surface);
            break;
        }
        case Platform::X11: {
            const VkXcbSurfaceCreateInfoKHR createInfo {
                .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
                .connection = static_cast<xcb_connection_t*>(platformInfo.displayHandle),
                .window = *static_cast<xcb_window_t*>(platformInfo.windowHandle)
            };
            vkCreateXcbSurfaceKHR(*instance, &createInfo, nullptr, surface);
            break;
        }
    }
}

// END LINUX PLATFORM CODE //
/////////////////////////////
#elif defined(__WIN32)
/////////////////////////////////
// BEGIN WINDOWS PLATFORM CODE //
#include <windows.h>
#include <vulkan/vulkan_win32.h>

void createSurface(const NativePlatformInfo& platformInfo, const vk::raii::Instance& instance, VkSurfaceKHR* surface) {
    const VkWin32SurfaceCreateInfoKHR createInfoWin32 {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .display = static_cast<HINSTANCE*>(platformInfo.displayHandle),
        .surface = static_cast<HWND*>(platformInfo.windowHandle)
    };
    vkCreateWin32SurfaceKHR(*instance, &createInfoWin32, nullptr, surface);
}

// END WINDOWS PLATFORM CODE //
///////////////////////////////
#endif
////////////////////////////////
// END PLATFORM-SPECIFIC CODE //
////////////////////////////////

RenderSurface::RenderSurface(const VulkanContext& context, const NativePlatformInfo& platformInfo)
    : mContext(context)
{
    const vk::raii::Instance& instance = context.getInstance();

    VkSurfaceKHR surface;
    createSurface(platformInfo, instance, &surface);

    mSurface = vk::raii::SurfaceKHR(instance, surface);
}
