//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "EventHandler2D.hpp"

#include <AMORA/algebra_types.hpp>
#include <GLFW/glfw3.h>

#include "../ChartisApplication.hpp"
#include "AMORA/logger.hpp"

#define SCREEN_TO_NDC(x, w) ((2.f * x / w) - 1.f)

using namespace chartis;

void EventHandler2D::processKeyEvent(Key key, Action action, Mods mods) {
    // Shift key bindings.
    if (key == Key::LEFT_SHIFT || key == Key::RIGHT_SHIFT) {
        if (action == Action::PRESS) {
            mIsShiftPressed = true;
        } else if (action == Action::RELEASE) {
            mIsShiftPressed = false;
        }
        updateCursor();
        return;
    }
    // Control key bindings.
    if (key == Key::LEFT_CONTROL || key == Key::RIGHT_CONTROL) {
        if (action == Action::PRESS) {
            mIsControlPressed = true;
        } else if (action == Action::RELEASE) {
            mIsControlPressed = false;
        }
        updateCursor();
        return;
    }
    // Alt key bindings.
    if (key == Key::LEFT_ALT || key == Key::RIGHT_ALT) {
        if (action == Action::PRESS) {
            mIsAltPressed = true;
        } else if (action == Action::RELEASE) {
            mIsAltPressed = false;
        }
        updateCursor();
        return;
    }

    // R resets the view.
    if (key == Key::R) {
        if (action == Action::PRESS) {
            mScene->mCamera.reset();
            mScene->invalidateCameraMatrices();
            mCallback();
        }
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
            const auto inverse = m_mat2x2(mScene->mCameraUBO.inverseMatrix);
            m_vec2 worldPos = inverse * m_vec2(x, y);
            m_vec2 lastWorldPos = inverse * m_vec2(lastX, lastY);

            if (mIsShiftPressed) {
                // SHEARING Y
                mScene->shear(0.f, (worldPos[1] - lastWorldPos[1]) / (worldPos[0] - mScene->mCamera.position[0]));
            } else {
                // SHEARING X
                mScene->shear((worldPos[0] - lastWorldPos[0]) / (worldPos[1] - mScene->mCamera.position[1]), 0.f);
            }
        } else if (mIsControlPressed) {
            // ROTATING
            const m_float delta = std::atan2(y, x) - std::atan2(lastY, lastX);

            mScene->rotate(delta);

            // Rotating also the current camera position around (0,0) so that
            // the rotation appears to be "in-place", i.e., centred around the
            // centre of the screen.

            const m_float s = std::sin(delta);
            const m_float c = std::cos(delta);
            const auto rotationMatrix = m_mat2x2(c, s, -s, c);

            mScene->mCamera.position = rotationMatrix * mScene->mCamera.position;
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
                mScene->translate(premultiply(m_vec2(x - lastX, y - lastY)));
            }
        }

        mScene->invalidateCameraMatrices();
        mCallback();
    }

    mLastX = xpos;
    mLastY = ypos;
}

void EventHandler2D::processMouseButtonEvent(MouseButton button, Action action, Mods mods) {
    if (button == MouseButton::LEFT) {
        if (action == Action::PRESS) {
            mIsLeftMouseButtonPressed = true;
        } else if (action == Action::RELEASE) {
            mIsLeftMouseButtonPressed = false;
        }
    } else if (button == MouseButton::RIGHT) {
        if (action == Action::PRESS) {
            mIsRightMouseButtonPressed = true;
        } else if (action == Action::RELEASE) {
            mIsRightMouseButtonPressed = false;
        }
    }
}

void EventHandler2D::processScrollEvent(double xoffset, double yoffset) {
    assert(mScene != nullptr);

    const m_float x = SCREEN_TO_NDC(mLastX, mSurfaceWidth);
    const m_float y = SCREEN_TO_NDC(mLastY, mSurfaceHeight);

    if (mIsControlPressed && !mIsAltPressed) {
        // ROTATING
        const m_float delta = yoffset / (2.f * std::numbers::pi);

        mScene->rotate(delta);

        // Rotating also the current camera position around (0,0) so that
        // the rotation appears to be "in-place", i.e., centred around the
        // centre of the screen.

        const m_float s = std::sin(delta);
        const m_float c = std::cos(delta);
        const auto rotationMatrix = m_mat2x2(c, s, -s, c);

        mScene->mCamera.position = rotationMatrix * (m_mat2x2(mScene->mCameraUBO.inverseMatrix) * m_vec2(x, y));
    } else {
        // Exponential scaling.
        const auto s = static_cast<m_float>(std::pow(1.15f, yoffset));
        mScene->zoom(s);

        // Scaling the translation so it appears centered.
        mScene->mCamera.position *= s;

        const m_vec2 cursorPosOffset = glm::inverse(m_mat2x2(mScene->mCameraUBO.projectionMatrix)) * m_vec2(x, y) * (1.f - s);
        mScene->mCamera.position += cursorPosOffset;
    }

    mScene->invalidateCameraMatrices();
    mCallback();
}

void EventHandler2D::resetMouse() {
    mLastX = POSITION_UNSET;
    mLastY = POSITION_UNSET;
    mIsLeftMouseButtonPressed = false;
    mIsRightMouseButtonPressed = false;
}

void EventHandler2D::updateCursor() const {
    if (!mIsAltPressed && !mIsControlPressed) {
        if (mIsShiftPressed) {
            // Only shift is pressed, so multiselect is enabled.
            mWindow.setCursor(Cursor::CROSSHAIR);
        } else {
            // Nothing is pressed: default cursor.
            mWindow.setCursor(Cursor::ARROW);
        }
    } else if (mIsControlPressed && !mIsAltPressed) {
        // Only control pressed: rotate. Shift makes no difference.
        mWindow.setCursor(Cursor::ROTATE);
    } else if (mIsAltPressed && !mIsControlPressed) { // First condition is redundant but kept for clarity.
        if (mIsShiftPressed) {
            // ALT + SHIFT: Scale vertical.
            mWindow.setCursor(Cursor::SCALE_VERTICAL);
        } else {
            // Only ALT: Scale horizontal.
            mWindow.setCursor(Cursor::SCALE_HORIZONTAL);
        }
    } else { // Both ALT and CONTROL are pressed.
        if (mIsShiftPressed) {
            // CONTROL + ALT + SHIFT: Shear vertical.
            mWindow.setCursor(Cursor::SHEAR_VERTICAL);
        } else {
            // Only ALT: Shear horizontal.
            mWindow.setCursor(Cursor::SHEAR_HORIZONTAL);
        }
    }
}
