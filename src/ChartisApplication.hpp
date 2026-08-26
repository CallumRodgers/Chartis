//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_CHARTISAPPLICATION_HPP
#define CHARTIS_CHARTISAPPLICATION_HPP

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>

#include "control/EventHandler2D.hpp"
#include "control/EventHandler3D.hpp"

#include "platypus/NativeWindow.hpp"

#ifdef NDEBUG
#define MAVIS_DEBUG(...)
#else
#define CHARTIS_DEBUG(...) LOG_DEBUG(chartis::logger, __VA_ARGS__)
#endif

#define CHARTIS_INFO(...) LOG_INFO(chartis::logger, __VA_ARGS__)
#define CHARTIS_WARN(...) LOG_WARNING(chartis::logger, __VA_ARGS__)
#define CHARTIS_ERROR(...) LOG_ERROR(chartis::logger, __VA_ARGS__)

namespace chartis {
    constexpr auto APPLICATION_VERSION = "0.1.0";

    /// Global logger for the engine.
    inline quill::Logger* logger = nullptr;

    static void beginLogging() {
        quill::BackendOptions options;
        options.sleep_duration = std::chrono::milliseconds{50};
        quill::Backend::start(options);

        const auto sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("Chartis_console_sink");
        logger = quill::Frontend::create_or_get_logger("Chartis", sink);

        CHARTIS_INFO("Started Chartis logging");
    }

    static void endLogging() {
        CHARTIS_INFO("Stopped Chartis logging");
        logger->flush_log();
        quill::Backend::stop();
    }

    class ChartisApplication {
    public:
        ChartisApplication(int argc, char** argv);

        void run();

        void invalidateSurface() { mSurfaceInvalidated = true; }
        void invalidateRender() { mRenderInvalidated = true; }

        platypus::NativeWindow mNativeWindow;

        EventHandler2D mEventHandler2D;
        EventHandler3D mEventHandler3D;

    private:
        bool mSurfaceInvalidated = false;

        bool mRenderInvalidated = false;

        /// Resizing operation doesn't need more than 30 FPS.
        int mMinResizeDelay = static_cast<int>(1.0f / 30.0f);
    };
}

#endif //CHARTIS_CHARTISAPPLICATION_HPP