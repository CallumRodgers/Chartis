//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_UICONTEXT_HPP
#define CHARTIS_UICONTEXT_HPP

#include "IconManager.hpp"
#include "ChartisApplication.hpp"

namespace chartis::interface {
    struct UIContext {
        ChartisApplication* app;
        const IconManager& iconManager;
    };
}

#endif //CHARTIS_UICONTEXT_HPP
