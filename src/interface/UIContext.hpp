////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

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
