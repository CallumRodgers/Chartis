////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_COMMANDDISPATCHER_HPP
#define OPENMAVIS_COMMANDDISPATCHER_HPP

#include <vulkan/vulkan_raii.hpp>

#include <AMORA/config.hpp>

#include <vector>

namespace amora {

    /**
     * The <code>CommandDispatcher</code> class encapsulates the managing of a queue, its respective command pool,
     * and the command buffers generated from it. The submission of commands happens through this class' public
     * member functions.
     */
    class CommandDispatcher {
    public:
        CommandDispatcher(const vk::raii::Device& device, const uint32_t familyIndex)
            :   mQueueIndex(familyIndex),
                mQueue(device, familyIndex, 0),
                mCommandPool(device, {
                    .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                    .queueFamilyIndex = familyIndex
                }),
                mCommandBuffers(vk::raii::CommandBuffers(device, {
                    .commandPool = mCommandPool,
                    .level = vk::CommandBufferLevel::ePrimary,
                    .commandBufferCount = config::rendering::MAX_FRAMES_IN_FLIGHT
                }))
        {}

        [[nodiscard]] uint32_t getFamilyIndex() const { return mQueueIndex; }
        [[nodiscard]] auto& getQueue() const { return mQueue; }
        [[nodiscard]] auto& getCommandBuffer(const int index) const { return mCommandBuffers[index]; }

    private:
        uint32_t mQueueIndex;
        vk::raii::Queue mQueue;
        vk::raii::CommandPool mCommandPool;
        std::vector<vk::raii::CommandBuffer> mCommandBuffers;

    };
}

#endif //OPENMAVIS_COMMANDDISPATCHER_HPP
