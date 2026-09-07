////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_SCENE2D_HPP
#define CHARTIS_SCENE2D_HPP

#include <map>

#include "Object2D.hpp"
#include "Scene.hpp"

namespace amora::scene {

    static const m_float NORMALIZATION = std::sqrt(2.f);

    class Scene2D final : public Scene {
    public:
        struct Camera2D {
            m_vec2 position;
            m_float globalScale;
            m_vec2 scales;
            m_float angle;
            m_vec2 shears;

            Camera2D() { reset(); }

            void reset() {
                position    = m_vec2(0.f, 0.f);
                globalScale = 1.f;
                scales      = m_vec2(1.f, 1.f);
                angle       = 0.f;
                shears      = m_vec2(0.f, 0.f);
            }

            [[nodiscard]] m_mat4x4 getViewMatrix() const {
                m_mat4x4 view = glm::lookAt(m_vec3(0.f, 0.f, 1.f), ORIGIN, Y_HAT);

                view = glm::translate(view, m_vec3(position, 0.f));
                view = glm::rotate(view, angle, Z_HAT);
                view = glm::shear(view, ORIGIN, m_vec2(shears.x, 0.f), m_vec2(shears.y, 0.f), m_vec2(0.f, 0.f));
                view = glm::scale(view, m_vec3(globalScale * scales, 1.f));

                return view;
            }
        };

        explicit Scene2D(const std::string& name);

        void addObject(uint32_t id, Object2D obj) {
            objects.insert(std::pair(id, obj));
        }
        void removeObject(uint32_t objId) {
            objects.erase(objId);
        }

        void translate(const m_vec2 a) {
            mCamera.position += a;
        }
        void rotate(const m_float angle) {
            mCamera.angle = std::fmod(mCamera.angle + angle, 2.f * static_cast<m_float>(std::numbers::pi));
        }
        void zoom(const m_float zoom) {
            mCamera.globalScale *= zoom;
        }
        void scale(const m_vec2 s) {
            mCamera.scales[0] *= s[0];
            mCamera.scales[1] *= s[1];

            const m_float globalScale = glm::length(mCamera.scales) / NORMALIZATION;
            mCamera.globalScale *= globalScale;
            mCamera.scales /= globalScale;
        }
        void shear(const m_float shearX, const m_float shearY) {
            mCamera.shears[0] += shearX;
            mCamera.shears[1] += shearY;
        }

        void invalidateCameraMatrices() override;

        void setupCameraMatrices(uint32_t surfaceWidth, uint32_t surfaceHeight) override;
        void updateSurfaceSize(uint32_t surfaceWidth, uint32_t surfaceHeight) override;

        void setupVulkan(const VulkanContext& context, const SwapChain& swapChain, DescriptorLayoutManager& layoutManager) override;
        void recordScene(const vk::raii::CommandBuffer &commandBuffer, uint32_t frameIndex) const override;

        Camera2D mCamera;

    private:
        std::map<uint32_t, Object2D> objects;
    };
}

#endif //CHARTIS_SCENE2D_HPP
