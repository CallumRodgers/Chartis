////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_VULKANCONTEXT_HPP
#define OPENMAVIS_VULKANCONTEXT_HPP

#include <vulkan/vulkan_raii.hpp>

namespace AMORA {

    /**
     * The VulkanContext class manages Vulkan instance creation, physical device selection,
     * and the respective creation of a logical device.
     */
    class VulkanContext {
    public:
        VulkanContext();

        void setupDevices(const vk::raii::SurfaceKHR& surface);

        [[nodiscard]] const auto& getInstance() const noexcept {
            return mInstance;
        }
        [[nodiscard]] const auto& getPhysicalDevice() const noexcept {
            return mPhysicalDevice;
        }
        [[nodiscard]] const auto& getDevice() const noexcept {
            return mDevice;
        }

        [[nodiscard]] uint32_t getGraphicsIndex() const { return mGraphicsIndex; }
        [[nodiscard]] uint32_t getComputeIndex() const { return mComputeIndex; }

    private:
        vk::raii::Context mContext;
        vk::raii::Instance mInstance = nullptr;
        vk::raii::PhysicalDevice mPhysicalDevice = nullptr;
        vk::raii::Device mDevice = nullptr;

        uint32_t mGraphicsIndex;
        uint32_t mComputeIndex;

        void pickPhysicalDevice();
        void createLogicalDevice(const vk::raii::SurfaceKHR& surface);
    };
}

#endif //OPENMAVIS_VULKANCONTEXT_HPP
