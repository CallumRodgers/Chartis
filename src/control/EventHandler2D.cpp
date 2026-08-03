//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "EventHandler2D.hpp"

#include <AMORA/algebra_types.hpp>
#include <GLFW/glfw3.h>

#include "../OpenMaVisApplication.hpp"

#define APP_POINTER(window) static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window))
#define SCREEN_TO_NDC(x, w) ((2.f * x / w) - 1.f)

using namespace mavis;

void EventHandler2D::processKeyEvent(int key, int scancode, int action, int mods) {
    // Shift key bindings.
    if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
        if (action == GLFW_PRESS) {
            mIsShiftPressed = true;
        } else if (action == GLFW_RELEASE) {
            mIsShiftPressed = false;
        }
        updateCursor();
        return;
    }
    // Control key bindings.
    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
        if (action == GLFW_PRESS) {
            mIsControlPressed = true;
        } else if (action == GLFW_RELEASE) {
            mIsControlPressed = false;
        }
        updateCursor();
        return;
    }
    // Alt key bindings.
    if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) {
        if (action == GLFW_PRESS) {
            mIsAltPressed = true;
        } else if (action == GLFW_RELEASE) {
            mIsAltPressed = false;
        }
        updateCursor();
        return;
    }
}

void EventHandler2D::processMouseMotionEvent(double xpos, double ypos) {
    // Both buttons at the same time defines no behaviour.
    if (mIsLeftMouseButtonPressed && mIsRightMouseButtonPressed) return;
    // Left mouse button actions are translating and rotating depending on the CONTROL mod.
    if (mIsLeftMouseButtonPressed) {
        assert(mLastX != POSITION_UNSET && mLastY != POSITION_UNSET);
        assert(mScene != nullptr);

        const m_float lastX = SCREEN_TO_NDC(mLastX, mSurfaceWidth);
        const m_float lastY = SCREEN_TO_NDC(mLastY, mSurfaceHeight);
        const m_float x     = SCREEN_TO_NDC(xpos, mSurfaceWidth);
        const m_float y     = SCREEN_TO_NDC(ypos, mSurfaceHeight);

        if (mIsControlPressed && mIsAltPressed) {
            // SHEARING
            constexpr m_vec2 noShear(0.f, 0.f);
            if (mIsShiftPressed) {
                // SHEARING Y
                const m_vec2 shearY(y - lastY, 0.f);
                mScene->shear(x * amora::scene::X_AXIS, noShear, shearY, noShear);
            } else {
                // SHEARING X
                const m_vec2 shearX(x - lastX, 0.f);
                mScene->shear(m_vec3(y, 0.f, 0.f), shearX, noShear, noShear);
            }
        } else if (mIsControlPressed) {
            // ROTATING
            const m_float delta = std::atan2(y, x) - std::atan2(lastY, lastX);

            mScene->rotate(amora::scene::Z_AXIS, delta);

            // Rotating also the current camera position around (0,0) so that
            // the rotation appears to be "in-place", i.e., centred around the
            // centre of the screen.

            const m_float s = std::sin(delta);
            const m_float c = std::cos(delta);
            const auto rotationMatrix = m_mat2x2(c, s, -s, c);

            m_mat4x4& view = mScene->mCameraUBO.viewMatrix;

            m_vec2 newCameraPos = rotationMatrix * m_vec2(view[3]);
            view[3][0] = newCameraPos[0];
            view[3][1] = newCameraPos[1];
        } else if (mIsAltPressed) {
            // SCALING
            const m_mat4x4& inverseTransform = mScene->mCameraUBO.inverseMatrix;

            auto cursorPos     = m_vec4(x, y, 0.f, 1.f);
            auto cursorLastPos = m_vec4(lastX, lastY, 0.f, 1.f);

            cursorPos     = inverseTransform * cursorPos;
            cursorLastPos = inverseTransform * cursorLastPos;

            if (mIsShiftPressed) {
                // SCALING Y
                mScene->scale(m_vec3(1.f, cursorPos[1] / cursorLastPos[1], 1.f));
            } else {
                // SCALING X
                mScene->scale(m_vec3(cursorPos[0] / cursorLastPos[0], 1.f, 1.f));
            }
        } else {
            if (mIsShiftPressed) {
                // MULTISELECT

            } else {
                // TRANSLATING
                mScene->translate(m_vec3(premultiply(m_vec2(x - lastX, y - lastY)), 0.0));
            }
        }

        mScene->invalidateCameraMatrices();
        APP_POINTER(mWindow)->invalidateRender();
    }

    mLastX = xpos;
    mLastY = ypos;
}

void EventHandler2D::processMouseButtonEvent(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mIsLeftMouseButtonPressed = true;
            glfwGetCursorPos(mWindow, &mLastX, &mLastY);
        } else if (action == GLFW_RELEASE) {
            mIsLeftMouseButtonPressed = false;
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mIsRightMouseButtonPressed = true;
        } else if (action == GLFW_RELEASE) {
            mIsRightMouseButtonPressed = false;
        }
    }
}

void EventHandler2D::processScrollEvent(double xoffset, double yoffset) {
    assert(mScene != nullptr);
    // Exponential scaling.
    const auto s = static_cast<m_float>(std::pow(1.15f, yoffset));
    mScene->scale(m_vec3(s, s, s));

    // Scaling the translation so it appears centered.
    m_mat4x4& view = mScene->mCameraUBO.viewMatrix;
    view[3][0] *= s;
    view[3][1] *= s;

    const m_float x = SCREEN_TO_NDC(mLastX, mSurfaceWidth);
    const m_float y = SCREEN_TO_NDC(mLastY, mSurfaceHeight);

    const m_vec2 cursorPosOffset = glm::inverse(m_mat2x2(mScene->mCameraUBO.projectionMatrix)) * m_vec2(x, y) * (1.f - s);
    view[3][0] += cursorPosOffset[0];
    view[3][1] += cursorPosOffset[1];

    mScene->invalidateCameraMatrices();
    APP_POINTER(mWindow)->invalidateRender();
}

void EventHandler2D::updateCursor() {
    if (!mIsAltPressed && !mIsControlPressed) {
        if (mIsShiftPressed) {
            // Only shift is pressed, so multiselect is enabled.
            glfwSetCursor(mWindow, OpenMaVisApplication::CURSOR_CROSSHAIR);
        } else {
            // Nothing is pressed: default cursor.
            glfwSetCursor(mWindow, OpenMaVisApplication::CURSOR_ARROW);
        }
    } else if (mIsControlPressed && !mIsAltPressed) {
        // Only control pressed: rotate. Shift makes no difference.
        glfwSetCursor(mWindow, OpenMaVisApplication::CURSOR_ROTATE);
    } else if (mIsAltPressed && !mIsControlPressed) { // First condition is redundant but kept for clarity.
        if (mIsShiftPressed) {
            // ALT + SHIFT: Scale vertical.
            glfwSetCursor(mWindow, OpenMaVisApplication::CURSOR_SCALE_VERTICAL);
        } else {
            // Only ALT: Scale horizontal.
            glfwSetCursor(mWindow, OpenMaVisApplication::CURSOR_SCALE_HORIZONTAL);
        }
    } else { // Both ALT and CONTROL are pressed.
        if (mIsShiftPressed) {
            // CONTROL + ALT + SHIFT: Shear vertical.
            glfwSetCursor(mWindow, OpenMaVisApplication::CURSOR_SHEAR_VERTICAL);
        } else {
            // Only ALT: Shear horizontal.
            glfwSetCursor(mWindow, OpenMaVisApplication::CURSOR_SHEAR_HORIZONTAL);
        }
    }
}
