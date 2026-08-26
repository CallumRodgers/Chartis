//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_PLATFORM_HPP
#define CHARTIS_PLATFORM_HPP

namespace chartis::platypus {
    enum class Platform {
        Windows,
        Wayland,
        X11,
        Cocoa
    };
    struct NativePlatformInfo {
        Platform platform;
        void* windowHandle;
        void* displayHandle;
    };
}

#endif //CHARTIS_PLATFORM_HPP
