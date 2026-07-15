////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_FENCEGROUP_HPP
#define OPENMAVIS_FENCEGROUP_HPP

#include <vulkan/vulkan_raii.hpp>

namespace AMORA {
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

#endif //OPENMAVIS_FENCEGROUP_HPP
