////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_PLATFORM_LOGGER_HPP
#define CHARTIS_PLATFORM_LOGGER_HPP

#include <quill/Logger.h>
#include <quill/LogMacros.h>

namespace chartis::platypus {
    /// Global logger for platform code.
    inline quill::Logger* logger = nullptr;
}

#ifdef NDEBUG
#define PLATYPUS_DEBUG(...)
#else
#define PLATYPUS_DEBUG(...) LOG_DEBUG(logger, __VA_ARGS__)
#endif

#define PLATYPUS_INFO(...) LOG_INFO(logger, __VA_ARGS__)
#define PLATYPUS_WARN(...) LOG_WARNING(logger, __VA_ARGS__)
#define PLATYPUS_ERROR(...) LOG_ERROR(logger, __VA_ARGS__)

#endif //CHARTIS_PLATFORM_LOGGER_HPP
