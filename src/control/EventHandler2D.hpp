//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_EVENTHANDLER2D_HPP
#define CHARTIS_EVENTHANDLER2D_HPP

#include <AMORA/Scene2D.hpp>
#include <GLFW/glfw3.h>

#include "platform/NativeWindow.hpp"

namespace chartis {
    /**
     *
     */
    class EventHandler2D {
    public:
        typedef std::function<void()> scene_update_callback;

        explicit EventHandler2D(const platform::NativeWindow& window) : mWindow(window) {}

        void setActiveScene(amora::scene::Scene2D* scene) { mScene = scene; }

        void updateSurfaceSize(const uint32_t w, const uint32_t h) {
            mSurfaceWidth = w;
            mSurfaceHeight = h;
        }

        void resetMouse();

        void setUpdateCallback(const scene_update_callback& callback) { mCallback = callback; }

        void processKeyEvent(Key key, Action action, Mods mods);
        void processMouseMotionEvent(double xpos, double ypos);
        void processMouseButtonEvent(MouseButton button, Action action, Mods mods);
        void processScrollEvent(double xoffset, double yoffset);

    private:
        constexpr static double POSITION_UNSET = -1.0;

        const platform::NativeWindow& mWindow;
        amora::scene::Scene2D* mScene;

        scene_update_callback mCallback;

        uint32_t mSurfaceWidth;
        uint32_t mSurfaceHeight;

        bool mIsLeftMouseButtonPressed = false;
        bool mIsRightMouseButtonPressed = false;
        bool mIsShiftPressed = false;
        bool mIsControlPressed = false;
        bool mIsAltPressed = false;

        double mLastX = POSITION_UNSET;
        double mLastY = POSITION_UNSET;

        [[nodiscard]] m_vec2 premultiply(const m_vec2& vec) const {
            return glm::inverse(m_mat2x2(mScene->mCameraUBO.projectionMatrix)) * vec;
        }

        void updateCursor() const;
    };
}

#endif //CHARTIS_EVENTHANDLER2D_HPP
