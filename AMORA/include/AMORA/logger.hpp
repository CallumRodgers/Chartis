////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_LOGGER_HPP
#define CHARTIS_LOGGER_HPP

#include <quill/Logger.h>
#include <quill/LogMacros.h>

namespace amora {
    /// Global logger for the engine.
    inline quill::Logger* logger = nullptr;
}

#ifdef NDEBUG
#define AMORA_DEBUG(...)
#else
#define AMORA_DEBUG(...) LOG_DEBUG(logger, __VA_ARGS__)
#endif

#define AMORA_INFO(...) LOG_INFO(logger, __VA_ARGS__)
#define AMORA_WARN(...) LOG_WARNING(logger, __VA_ARGS__)
#define AMORA_ERROR(...) LOG_ERROR(logger, __VA_ARGS__)

#endif //CHARTIS_LOGGER_HPP
