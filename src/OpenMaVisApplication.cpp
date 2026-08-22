//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "OpenMaVisApplication.hpp"

#include "interface/UserInterface.hpp"
#include "backends/imgui_impl_glfw.h"

#include <AMORA/Engine.hpp>
#include <AMORA/Scene2D.hpp>
#include "file_utils.hpp"
#include "key_converter.hpp"
#include "interface/IconManager.hpp"

using namespace mavis;

OpenMaVisApplication::OpenMaVisApplication(int argc, char** argv)
    : mEventHandler2D(mNativeWindow)
        //mEventHandler3D(mWindow),
{
    auto [w, h] = mNativeWindow.getWindowSize();

    mEventHandler2D.setUpdateCallback([this]() -> void {
        invalidateRender();
    });
    mEventHandler2D.updateSurfaceSize(w, h);

    ////////////////////////////////
    // SETUP USER INPUT CALLBACKS //
    ////////////////////////////////

    mNativeWindow.setWindowResizeCallback([this](int width, int height) -> void {
        invalidateSurface();
    });
    mNativeWindow.setPointerEnterCallback([this](double x, double y) -> void {
        ImGui::GetIO().AddMousePosEvent(static_cast<float>(x), static_cast<float>(y));
        mEventHandler2D.processMouseMotionEvent(x, y);
        mNativeWindow.setCursor(Cursor::ARROW);
    });
    mNativeWindow.setPointerLeaveCallback([this]() -> void {
        ImGui::GetIO().ClearInputMouse();
        mEventHandler2D.resetMouse();

        invalidateRender();
    });
    mNativeWindow.setPointerMoveCallback([this](double xpos, double ypos) -> void {
        auto& io = ImGui::GetIO();
        io.AddMousePosEvent(static_cast<float>(xpos), static_cast<float>(ypos));

        if (!io.WantCaptureMouse) {
            mEventHandler2D.processMouseMotionEvent(xpos, ypos);
        }

        invalidateRender();
    });
    mNativeWindow.setPointerButtonCallback([this](MouseButton button, Action action, Mods mods) -> void {
        auto& io = ImGui::GetIO();

        ImGuiMouseButton imGuiButton;
        switch (button) {
            default:
            case MouseButton::LEFT: imGuiButton = ImGuiMouseButton_Left; break;
            case MouseButton::MIDDLE: imGuiButton = ImGuiMouseButton_Middle; break;
            case MouseButton::RIGHT: imGuiButton = ImGuiMouseButton_Right; break;
        }

        io.AddMouseButtonEvent(imGuiButton, action == Action::PRESS);

        if (!io.WantCaptureMouse) {
            mEventHandler2D.processMouseButtonEvent(button, action, mods);
        }

        invalidateRender();
    });
    mNativeWindow.setPointerScrollCallback([this](double xOffset, double yOffset) -> void {
        auto& io = ImGui::GetIO();
        io.AddMouseWheelEvent(xOffset, yOffset);

        if (!io.WantCaptureMouse) {
            mEventHandler2D.processScrollEvent(xOffset, yOffset);
        }

        invalidateRender();
    });
    mNativeWindow.setKeyCallback([this](Key key, Action action, Mods mods) -> void {
        if (key == Key::F11 && action == Action::PRESS && mods == 0) {
            mNativeWindow.toggleFullscreen();
        }

        auto& io = ImGui::GetIO();
        if (action == Action::PRESS) {
            io.AddKeyEvent(keyToImGuiKey(key), true);
        } else if (action == Action::RELEASE) {
            io.AddKeyEvent(keyToImGuiKey(key), false);
        }

        if (!io.WantCaptureKeyboard) {
            mEventHandler2D.processKeyEvent(key, action, mods);
        }

        invalidateRender();
    });

    //createCursors(mWindow);
}

void OpenMaVisApplication::run() {
    amora::Engine::startLogging();

    auto [windowWidth, windowHeight] = mNativeWindow.getWindowSize();
    // By creating the Engine object here, it'll be automatically cleaned up
    // once it goes out of scope (i.e., the main loop exits).
    const amora::Engine::EngineInitInfo initInfo = {
        .platformInfo = mNativeWindow.getNativePlatformInfo(),
        .surfaceWidth = windowWidth,
        .surfaceHeight = windowHeight
    };
    amora::Engine engine(initInfo);
    const amora::ImGuiEngineData data = engine.beginInterfaceBackend();

    auto iconManager = interface::IconManager(engine);
    iconManager.loadIcons();

    using interface::UserInterface;
    // Sets up the ImGui context on the application side, and sets up
    // interface styling, loads fonts, icons, etc.
    UserInterface ui(data, iconManager);
    ImGui::GetIO().DisplaySize = ImVec2(initInfo.surfaceWidth, initInfo.surfaceHeight);

    auto scene = amora::scene::Scene2D("Default");
    engine.loadScene(scene, true);

    mEventHandler2D.setActiveScene(&scene);

    // For our purposes we don't need to be rendering at V-Sync FPS non-stop if the
    // user is not interacting with us; we can idle the GPU usage to 0% after some frames
    // since the last input. The exception is if a scene is currently under animation,
    // in which case the engine will tell us, and the application will render continuously.
    constexpr int minContinuousFrames = 3;
    int frameIndex = 0;
    bool waitEvents = true;
    // Obviously we want to render the first frame.
    bool firstFrame = true;
    mRenderInvalidated = true;

    /////////////////////
    // BEGIN MAIN LOOP //
    /////////////////////
    while (firstFrame || !waitEvents || !mNativeWindow.isClosing()) {
        if (waitEvents && !firstFrame) {
            // If we're here, it means the user has just interacted with the application.
            // We must therefore render minContinuousFrames to give time for the UI to fully stabilise.
            waitEvents = false;
        }

        if (!waitEvents) {
            // If we're not waiting for events, we should poll them ourselves to ensure
            // the application still receives inputs.
            mNativeWindow.pollEvents();
        }

        if (mSurfaceInvalidated) {
            auto [w, h] = mNativeWindow.getWindowSize();
            engine.invalidateSurfaceSize(w, h);
            mEventHandler2D.updateSurfaceSize(w, h);
            mRenderInvalidated = true;
            ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
            mSurfaceInvalidated = false;
        }

        ////////////////////////////
        // RENDERING SCENE AND UI //
        ////////////////////////////

        if (mRenderInvalidated) {
            // Sets everything up for a new frame to be drawn.
            engine.beginFrame();
            ui.layout(this);
            // Submits the draw commands to the queue and presents the image.
            engine.endFrame();

            // Updating frame variables and checking if we should idle the application.
            frameIndex++;
            if (frameIndex == minContinuousFrames) {
                frameIndex = 0;
                waitEvents = true;
                mRenderInvalidated = false;
            }
        }

        if (firstFrame) {
            firstFrame = false;
        }
    }
    ///////////////////
    // END MAIN LOOP //
    ///////////////////

    engine.waitIdle();
    engine.endInterfaceBackend();
}
