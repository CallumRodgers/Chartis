////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_VULKAN_UTILS_HPP
#define CHARTIS_VULKAN_UTILS_HPP

#include <vulkan/vulkan_raii.hpp>

namespace amora::vulkan_utils {
    static uint32_t findMemoryType(const vk::raii::PhysicalDevice& physicalDevice,
                                   const uint32_t typeFilter,
                                   const vk::MemoryPropertyFlags properties) {
        const vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
            // Checks if the "i"th bit is set.
            if (typeFilter & (1 << i)) {
                // Checks if the "i"th memory type's properties agree with all required properties.
                if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }
        }
        throw std::runtime_error("Failed to find suitable memory type.");
    }
}

#endif //CHARTIS_VULKAN_UTILS_HPP
