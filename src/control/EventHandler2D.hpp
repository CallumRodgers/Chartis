//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_EVENTHANDLER2D_HPP
#define OPENMAVIS_EVENTHANDLER2D_HPP

#include <AMORA/Scene2D.hpp>
#include <GLFW/glfw3.h>

namespace mavis {
    /**
     *
     */
    class EventHandler2D {
    public:
        explicit EventHandler2D(GLFWwindow* window) : mWindow(window) {}

        void setActiveScene(amora::scene::Scene2D* scene) { mScene = scene; }

        void updateSurfaceSize(const uint32_t w, const uint32_t h) {
            mSurfaceWidth = w;
            mSurfaceHeight = h;
        }

        void processKeyEvent(int key, int scancode, int action, int mods);
        void processMouseMotionEvent(double xpos, double ypos);
        void processMouseButtonEvent(int button, int action, int mods);
        void processScrollEvent(double xoffset, double yoffset);

    private:
        constexpr static double POSITION_UNSET = -1.0;

        GLFWwindow* mWindow;
        amora::scene::Scene2D* mScene;

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

#endif //OPENMAVIS_EVENTHANDLER2D_HPP
