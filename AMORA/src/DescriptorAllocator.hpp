//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_DESCRIPTORALLOCATOR_HPP
#define OPENMAVIS_DESCRIPTORALLOCATOR_HPP

#include <vulkan/vulkan_raii.hpp>

#include "VulkanContext.hpp"

namespace amora {
    class DescriptorAllocator {
    public:
        explicit DescriptorAllocator(const VulkanContext& context);

        [[nodiscard]] std::vector<vk::raii::DescriptorSet> allocate(const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts) const;

    private:
        const vk::raii::Device& mDevice;

        vk::DescriptorPoolSize mPoolSize;
        vk::raii::DescriptorPool mPool {nullptr};
    };
}

#endif //OPENMAVIS_DESCRIPTORALLOCATOR_HPP
