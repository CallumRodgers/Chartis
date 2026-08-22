//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_UICONTEXT_HPP
#define OPENMAVIS_UICONTEXT_HPP

#include "IconManager.hpp"
#include "OpenMaVisApplication.hpp"

namespace mavis::interface {
    struct UIContext {
        OpenMaVisApplication* app;
        const IconManager& iconManager;
    };
}

#endif //OPENMAVIS_UICONTEXT_HPP
