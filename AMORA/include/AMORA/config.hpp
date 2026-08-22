//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_CONFIG_HPP
#define OPENMAVIS_CONFIG_HPP

#include <cstdint>
#include <vector>

// CTRL+C -> CTRL+V of Vulkan's macro.
#define MAKE_VERSION(major, minor, patch) (((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch))

namespace amora::config {

    namespace info {
        /// The name of our application, given to the <code>vk::raii::Instance</code> object.
        constexpr auto APPLICATION_NAME = "OpenMaVis";
        /// The current app version. It may be different from the engine version.
        constexpr auto APPLICATION_VERSION = MAKE_VERSION(0, 1, 0);
        /// "Official" name for this Vulkan engine, given to the <code>vk::raii::Instance</code> object.
        constexpr auto ENGINE_NAME = "AMORA Engine";
        /// Ideally this is the only variable project-wide that contains the Engine version.
        constexpr auto ENGINE_VERSION = MAKE_VERSION(0, 1, 0);
        /// Vulkan version.
        constexpr auto API_VERSION = 4210688;
    }

    namespace layers {
        const std::vector VALIDATION_LAYERS = {
            "VK_LAYER_KHRONOS_validation"
        };

#if NDEBUG
        constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
        constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif
    }

    namespace extensions {
        const std::vector REQUIRED_DEVICE_EXTENSIONS = {
            "VK_KHR_swapchain"
        };
    }

    namespace rendering {
        constexpr float GRAPHICS_QUEUE_PRIORITY = 0.6f;
        constexpr float COMPUTE_QUEUE_PRIORITY = 0.4f;

        constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

        constexpr uint32_t MAX_BINDING_SET_INDEX = 3;
        constexpr uint32_t MAX_BINDING_INDEX = 15;
    }

}

#endif //OPENMAVIS_CONFIG_HPP
