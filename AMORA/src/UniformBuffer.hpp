//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_UNIFORMBUFFER_HPP
#define OPENMAVIS_UNIFORMBUFFER_HPP

#include <vulkan/vulkan_raii.hpp>

#include "AMORA/Scene.hpp"
#include "VulkanContext.hpp"

namespace amora {
    template <class UBO> class UniformBuffer {
    public:
        explicit UniformBuffer(const VulkanContext& context);

        /**
         * Uploads a Uniform Buffer Object to the GPU.
         * @param bufferData The data to be uploaded.
         */
        void uploadData(const UBO& bufferData) const;

        void unmapMemory();

        [[nodiscard]] vk::DescriptorBufferInfo getDescriptorBufferInfo() const {
            return vk::DescriptorBufferInfo {
                .buffer = mBuffer,
                .offset = 0,
                .range = mBufferObjectSize
            };
        }

        [[nodiscard]] const vk::raii::Buffer& getBuffer() const noexcept { return mBuffer; }

    private:
        vk::raii::Buffer mBuffer {nullptr};
        vk::raii::DeviceMemory mBufferMemory {nullptr};
        void* mBufferMap;

        vk::DeviceSize mBufferObjectSize;
    };

    // Stating which templates we want to compile against, otherwise we get linker errors.
    template class UniformBuffer<scene::Scene::CameraUBO>;
}

#endif //OPENMAVIS_UNIFORMBUFFER_HPP
