//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_OPENMAVISAPPLICATION_HPP
#define OPENMAVIS_OPENMAVISAPPLICATION_HPP

#include <GLFW/glfw3.h>

#include <quill/LogMacros.h>
#include <quill/Logger.h>
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>

#include "control/EventHandler2D.hpp"
#include "control/EventHandler3D.hpp"

#ifdef NDEBUG
#define MAVIS_DEBUG(...)
#else
#define MAVIS_DEBUG(...) LOG_DEBUG(mavis::logger, __VA_ARGS__)
#endif

#define MAVIS_INFO(...) LOG_INFO(mavis::logger, __VA_ARGS__)
#define MAVIS_WARN(...) LOG_WARNING(mavis::logger, __VA_ARGS__)
#define MAVIS_ERROR(...) LOG_ERROR(mavis::logger, __VA_ARGS__)

namespace mavis {
    constexpr auto APPLICATION_VERSION = "0.1.0";

    /// Global logger for the engine.
    inline quill::Logger* logger = nullptr;

    static void beginLogging() {
        quill::BackendOptions options;
        options.sleep_duration = std::chrono::milliseconds{50};
        quill::Backend::start(options);

        const auto sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("OpenMaVis_console_sink");
        logger = quill::Frontend::create_or_get_logger("OpenMaVis", sink);

        MAVIS_INFO("Started OpenMaVis logging");
    }

    static void endLogging() {
        MAVIS_INFO("Stopped OpenMaVis logging");
        logger->flush_log();
        quill::Backend::stop();
    }

    /** Static callback functions for user input that are directly invoked by GLFW. */
    namespace callbacks {
        static void onFramebufferResizeCallback (GLFWwindow* window, int width, int height);
        static void onKeyEventCallback          (GLFWwindow* window, int key, int scancode, int action, int mods);
        static void onMouseMotionEventCallback  (GLFWwindow* window, double xpos, double ypos);
        static void onMouseButtonEventCallback  (GLFWwindow* window, int button, int action, int mods);
        static void onScrollEventCallback       (GLFWwindow* window, double xoffset, double yoffset);
    }

    class OpenMaVisApplication {
    public:
        inline static GLFWcursor* CURSOR_ARROW = nullptr;
        inline static GLFWcursor* CURSOR_ROTATE = nullptr;
        inline static GLFWcursor* CURSOR_CROSSHAIR = nullptr;
        inline static GLFWcursor* CURSOR_SCALE_HORIZONTAL = nullptr;
        inline static GLFWcursor* CURSOR_SCALE_VERTICAL = nullptr;
        inline static GLFWcursor* CURSOR_SHEAR_HORIZONTAL = nullptr;
        inline static GLFWcursor* CURSOR_SHEAR_VERTICAL = nullptr;

        GLFWwindow* mWindow;

        EventHandler2D mEventHandler2D;
        EventHandler3D mEventHandler3D;

        OpenMaVisApplication(int argc, char** argv);

        void run();
        void terminate();

        void invalidateSurface() { mSurfaceInvalidated = true; }
        void invalidateRender() { mRenderInvalidated = true; }

        void toggleFullscreenMode();

    private:
        bool mSurfaceInvalidated = false;

        bool mRenderInvalidated = false;

        int mLastWindowX = 0;
        int mLastWindowY = 0;
        int mLastWindowWidth = 0;
        int mLastWindowHeight = 0;

        /// Resizing operation doesn't need more than 30 FPS.
        int mMinResizeDelay = static_cast<int>(1.0f / 30.0f);

        static GLFWwindow* createWindow();
        static void createCursors(GLFWwindow* window);
    };
}

#endif //OPENMAVIS_OPENMAVISAPPLICATION_HPP