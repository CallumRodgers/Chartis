////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_DESCRIPTORALLOCATOR_HPP
#define CHARTIS_DESCRIPTORALLOCATOR_HPP

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

#endif //CHARTIS_DESCRIPTORALLOCATOR_HPP
