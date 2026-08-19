//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_OPENMAVISAPPLICATION_HPP
#define OPENMAVIS_OPENMAVISAPPLICATION_HPP

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>

#include "control/EventHandler2D.hpp"
#include "control/EventHandler3D.hpp"

#include "platform/NativeWindow.hpp"

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

    class OpenMaVisApplication {
    public:
        OpenMaVisApplication(int argc, char** argv);

        void run();

        void invalidateSurface() { mSurfaceInvalidated = true; }
        void invalidateRender() { mRenderInvalidated = true; }

        platform::NativeWindow mNativeWindow;

        EventHandler2D mEventHandler2D;
        EventHandler3D mEventHandler3D;

    private:
        bool mSurfaceInvalidated = false;

        bool mRenderInvalidated = false;

        /// Resizing operation doesn't need more than 30 FPS.
        int mMinResizeDelay = static_cast<int>(1.0f / 30.0f);
    };
}

#endif //OPENMAVIS_OPENMAVISAPPLICATION_HPP