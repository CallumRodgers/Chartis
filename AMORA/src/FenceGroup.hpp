////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_FENCEGROUP_HPP
#define CHARTIS_FENCEGROUP_HPP

#include <vulkan/vulkan_raii.hpp>

namespace amora {
    class FenceGroup {
    public:
        FenceGroup(const vk::raii::Device& device, const uint32_t amount) {
            for (uint32_t i = 0; i < amount; i++) {
                mFences.emplace_back(device, vk::FenceCreateInfo {
                    .flags = vk::FenceCreateFlagBits::eSignaled
                });
            }
        }

        const vk::raii::Fence& operator[](const size_t index) { return mFences[index]; }

    private:
        std::vector<vk::raii::Fence> mFences;
    };
}

#endif //CHARTIS_FENCEGROUP_HPP
