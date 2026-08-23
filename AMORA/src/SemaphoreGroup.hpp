//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_SEMAPHOREGROUP_HPP
#define CHARTIS_SEMAPHOREGROUP_HPP

#include <vulkan/vulkan_raii.hpp>

namespace amora {
    class SemaphoreGroup {
    public:
        SemaphoreGroup(const vk::raii::Device& device, const uint32_t amount) {
            for (uint32_t i = 0; i < amount; i++) {
                mSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
            }
        }

        const vk::raii::Semaphore& operator[](const size_t index) { return mSemaphores[index]; }

    private:
        std::vector<vk::raii::Semaphore> mSemaphores;
    };
}

#endif //CHARTIS_SEMAPHOREGROUP_HPP
