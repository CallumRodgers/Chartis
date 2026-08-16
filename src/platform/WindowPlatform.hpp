//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_WINDOWPLATFORM_HPP
#define OPENMAVIS_WINDOWPLATFORM_HPP

#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace mavis::platform {
    class WindowPlatform {
    public:
        WindowPlatform() {
        }
    };
}

#endif //OPENMAVIS_WINDOWPLATFORM_HPP
