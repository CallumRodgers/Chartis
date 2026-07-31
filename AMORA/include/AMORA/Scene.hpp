//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_SCENE_HPP
#define OPENMAVIS_SCENE_HPP

#include <memory>
#include <string>
#include <AMORA/algebra_types.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FrameGroup.hpp"

namespace amora {
    class VulkanContext;
    class SwapChain;
    template <class UBO> class UniformBuffer;
    class DescriptorAllocator;
    class DescriptorLayoutManager;
    class DescriptorSetData;
}

namespace amora::graphics {
    class GraphicsPipeline;
}

namespace vk::raii {
    class CommandBuffer;
}

namespace amora::scene {

    constexpr m_float PROJECTION_SCALE = 1.f;

    constexpr auto ORIGIN = m_vec3(0.f, 0.f, 0.f);
    constexpr auto X_AXIS = m_vec3(1.f, 0.f, 0.f);
    constexpr auto Y_AXIS = m_vec3(0.f, 1.f, 0.f);
    constexpr auto Z_AXIS = m_vec3(0.f, 0.f, 1.f);

    class Scene {
    public:
        struct CameraBufferObject {
            m_mat4x4 projectionMatrix;
            m_mat4x4 viewMatrix;
        };

        virtual ~Scene();

        void translate(const m_vec3 a)                      { mCameraUBO.viewMatrix = glm::translate(mCameraUBO.viewMatrix, a); }
        void rotate(const m_vec3 axis, const m_float angle) { mCameraUBO.viewMatrix = glm::rotate(mCameraUBO.viewMatrix, angle, axis); }
        void scale(const m_vec3 s)                          { mCameraUBO.viewMatrix = glm::scale(mCameraUBO.viewMatrix, s); }
        void shear(const m_vec3 p, const m_vec2 lX, const m_vec2 lY, const m_vec2 lZ) {
            mCameraUBO.viewMatrix = glm::shear(mCameraUBO.viewMatrix, p, lX, lY, lZ);
        }

        virtual void setupCameraMatrices(uint32_t surfaceWidth, uint32_t surfaceHeight) = 0;
        virtual void updateSurfaceSize(uint32_t surfaceWidth, uint32_t surfaceHeight) = 0;

        virtual void setupVulkan(const VulkanContext& context, const SwapChain& swapChain, DescriptorLayoutManager& layoutManager) = 0;
        virtual void recordScene(const vk::raii::CommandBuffer& commandBuffer, uint32_t frameIndex) const = 0;

        CameraBufferObject mCameraUBO;

    protected:
        Scene() = default;


        uint32_t surfaceWidth;
        uint32_t surfaceHeight;

        std::string mSceneName;

        std::unique_ptr<graphics::GraphicsPipeline> mBackgroundPipeline;
        std::unique_ptr<graphics::GraphicsPipeline> mAxesPipeline;
        std::unique_ptr<DescriptorAllocator> mDescriptorAllocator;

        ///////////////////////////////////
        // PER-FRAME IN FLIGHT RESOURCES //
        ///////////////////////////////////

        FrameGroup<std::unique_ptr<UniformBuffer<CameraBufferObject>>> mCameraBuffers;
        FrameGroup<std::unique_ptr<DescriptorSetData>> mDescriptorSets;

    };
}

#endif //OPENMAVIS_SCENE_HPP
