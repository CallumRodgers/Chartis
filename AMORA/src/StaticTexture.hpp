////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_TEXTURE_HPP
#define CHARTIS_TEXTURE_HPP

#include <vulkan/vulkan_raii.hpp>

#include "CommandDispatcher.hpp"
#include "VulkanContext.hpp"

namespace amora {
    /**
     * A <code>StaticTexture</code> object is a Vulkan image containing texture data that is constant throughout
     * the application's lifetime. It should not be used as a render target.
     */
    class StaticTexture {
    public:
        /// Creates a new StaticTexture from a RGBA array of pixels (e.g. from stb_image).
        /// Note that <code>StaticTexture::prepare</code> <i>must</i> be called before usage.
        StaticTexture(const VulkanContext& device, int width, int height, unsigned char* pixels);

        void prepare(const CommandDispatcher& dispatcher) const;

        [[nodiscard]] const vk::raii::ImageView& getImageView() const noexcept { return mImageView; }

    private:
        void transitionLayout(const vk::raii::CommandBuffer& commandBuffer,
                              vk::ImageLayout oldLayout,
                              vk::ImageLayout newLayout) const;

        uint32_t mWidth, mHeight;

        vk::raii::Image mImage {nullptr};
        vk::raii::DeviceMemory mImageMemory {nullptr};
        vk::raii::ImageView mImageView {nullptr};

        vk::raii::Buffer mStagingBuffer {nullptr};
        vk::raii::DeviceMemory mStagingBufferMemory {nullptr};
    };
}

#endif //CHARTIS_TEXTURE_HPP
