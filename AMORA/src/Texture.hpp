//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_TEXTURE_HPP
#define OPENMAVIS_TEXTURE_HPP

#include <vulkan/vulkan_raii.hpp>

#include "VulkanContext.hpp"

namespace amora {
    class Texture {
    public:
        Texture(const VulkanContext& device, int width, int height, unsigned char* pixels);

    private:
        vk::raii::Image mImage {nullptr};
        vk::raii::DeviceMemory mImageMemory {nullptr};
    };
}

#endif //OPENMAVIS_TEXTURE_HPP
