//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_ICONS_HPP
#define OPENMAVIS_ICONS_HPP

#include <unordered_map>
#include "imgui.h"
#include "AMORA/AMORA.hpp"

namespace mavis::interface {
    typedef ImTextureID IconImage;

    enum Icon {
        /////////////////////////
        // MAIN MENU BAR ICONS //
        /////////////////////////
        MINIMISE, MAXIMISE, COLLAPSE, CLOSE,
        MENU, PLUS,

        ////////////////////
        // SIDE BAR ICONS //
        ////////////////////
        TREE, PLAY_PAUSE, CALCULATOR, LOG, SETTINGS
    };

    class IconManager {
    public:
        explicit IconManager(const amora::Engine& engine) : mEngine(engine) {}

        void loadIcons();

        IconImage getIconImage(Icon icon) const {
            return mIcons.at(icon);
        }

    private:
        void put(Icon icon, const char* filePath);

        static std::string getId(Icon icon) {
            return "Icon" + std::to_string(icon);
        }

        const amora::Engine& mEngine;

        std::unordered_map<Icon, IconImage> mIcons;
    };
};

#endif //OPENMAVIS_ICONS_HPP
