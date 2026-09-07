////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_SCENE_HPP
#define CHARTIS_SCENE_HPP

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

namespace vk::raii {
    class CommandBuffer;
}

/**
 *
 */
namespace amora::scene {

    class Background;
    class CoordinateGrid;

    constexpr m_float PROJECTION_SCALE = 5.f;

    constexpr auto ORIGIN = m_vec3(0.f, 0.f, 0.f);
    constexpr auto X_HAT = m_vec3(1.f, 0.f, 0.f);
    constexpr auto Y_HAT = m_vec3(0.f, 1.f, 0.f);
    constexpr auto Z_HAT = m_vec3(0.f, 0.f, 1.f);

    /**
     *
     */
    class Scene {
    public:
        struct CameraUBO {
            m_mat4x4 projectionMatrix;
            m_mat4x4 viewMatrix;
            m_mat4x4 inverseMatrix;
        };

        virtual ~Scene();

        /**
         * Ensures that the view matrix is "within bounds" like the [min, max] scale range,
         * and within the maximum shearing limit, etc. It also computes the total inverse transformation
         * matrix for the current proj * view.
         */
        virtual void invalidateCameraMatrices() = 0;

        virtual void setupCameraMatrices(uint32_t surfaceWidth, uint32_t surfaceHeight) = 0;
        virtual void updateSurfaceSize(uint32_t surfaceWidth, uint32_t surfaceHeight) = 0;

        virtual void setupVulkan(const VulkanContext& context, const SwapChain& swapChain, DescriptorLayoutManager& layoutManager) = 0;
        virtual void recordScene(const vk::raii::CommandBuffer& commandBuffer, uint32_t frameIndex) const = 0;

        CameraUBO mCameraUBO;

    protected:
        Scene() = default;

        uint32_t surfaceWidth;
        uint32_t surfaceHeight;

        std::string mSceneName;

        std::unique_ptr<Background> mBackground;
        std::unique_ptr<CoordinateGrid> mCoordinateGrid;

        std::unique_ptr<DescriptorAllocator> mDescriptorAllocator;

        ///////////////////////////////////
        // PER-FRAME IN FLIGHT RESOURCES //
        ///////////////////////////////////

        FrameGroup<std::unique_ptr<UniformBuffer<CameraUBO>>> mCameraBuffers;
        FrameGroup<std::unique_ptr<DescriptorSetData>> mDescriptorSets;
    };
}

#endif //CHARTIS_SCENE_HPP
