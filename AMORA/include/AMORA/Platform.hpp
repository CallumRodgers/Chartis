//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_PLATFORM_HPP
#define OPENMAVIS_PLATFORM_HPP

namespace amora {
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

#endif //OPENMAVIS_PLATFORM_HPP
