//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_SCENE2D_HPP
#define OPENMAVIS_SCENE2D_HPP

#include <map>

#include "Object2D.hpp"
#include "Scene.hpp"

namespace amora::scene {

    class Scene2D final : public Scene {
    public:
        explicit Scene2D(const std::string& name);

        void addObject(uint32_t id, Object2D obj) {
            objects.insert(std::pair(id, obj));
        }
        void removeObject(uint32_t objId) {
            objects.erase(objId);
        }

        void setupCameraMatrices(uint32_t surfaceWidth, uint32_t surfaceHeight) override;
        void updateSurfaceSize(uint32_t surfaceWidth, uint32_t surfaceHeight) override;

        void setupVulkan(const VulkanContext& context, const SwapChain& swapChain, DescriptorLayoutManager& layoutManager) override;
        void recordScene(const vk::raii::CommandBuffer &commandBuffer, uint32_t frameIndex) const override;

    private:
        std::map<uint32_t, Object2D> objects;
    };
}

#endif //OPENMAVIS_SCENE2D_HPP
