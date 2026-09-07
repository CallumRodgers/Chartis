////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include "IconManager.hpp"
#include <string>

using namespace chartis::interface;

void IconManager::put(Icon icon, const char* filePath) {
    mIcons.emplace(icon, mEngine.loadIcon(getId(icon), filePath));
}

void IconManager::loadIcons() {
    put(MINIMISE,   "res/icons/minimise.png");
    put(MAXIMISE,   "res/icons/maximise.png");
    put(COLLAPSE,   "res/icons/collapse.png");
    put(CLOSE,      "res/icons/close.png");
    put(MENU,       "res/icons/menu.png");
    put(PLUS,       "res/icons/plus.png");

    put(TREE,       "res/icons/tree.png");
    put(PLAY_PAUSE, "res/icons/play_pause.png");
    put(CALCULATOR, "res/icons/calculator.png");
    put(LOG,        "res/icons/log.png");
    put(SETTINGS,   "res/icons/settings.png");
}
