////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_DESCRIPTORSETDATA_HPP
#define CHARTIS_DESCRIPTORSETDATA_HPP

#include <vulkan/vulkan_raii.hpp>
#include "DescriptorAllocator.hpp"

namespace amora {
    class DescriptorSetData {
    public:
        DescriptorSetData(const std::vector<vk::DescriptorSetLayout>& layouts, const DescriptorAllocator& allocator);

        void linkToBuffer(const vk::DescriptorBufferInfo& bufferInfo, uint32_t dstSet, uint32_t dstBinding) const;

        [[nodiscard]] const std::vector<vk::raii::DescriptorSet>& getDescriptorSets() const { return mDescriptorSets; }

    private:
        std::vector<vk::raii::DescriptorSet> mDescriptorSets;
    };
}

#endif //CHARTIS_DESCRIPTORSETDATA_HPP
