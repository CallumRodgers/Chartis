////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_LOGGER_HPP
#define OPENMAVIS_LOGGER_HPP

#include <quill/LogMacros.h>
#include <quill/Logger.h>

namespace AMORA {
    /// Global logger for the engine.
    inline quill::Logger* logger = nullptr;
}

#define ENGINE_INFO(...) LOG_INFO(logger, __VA_ARGS__)
#define ENGINE_WARN(...) LOG_WARNING(logger, __VA_ARGS__)
#define ENGINE_ERROR(...) LOG_ERROR(logger, __VA_ARGS__)

#endif //OPENMAVIS_LOGGER_HPP
