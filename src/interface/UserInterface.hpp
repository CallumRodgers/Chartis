//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_USERINTERFACE_HPP
#define OPENMAVIS_USERINTERFACE_HPP

#include <AMORA/ImGuiEngineData.hpp>

#include "IconManager.hpp"
#include "MainBar.hpp"
#include "OpenMaVisApplication.hpp"
#include "SideBar.hpp"

namespace mavis::interface {
    class UserInterface {
    public:
        explicit UserInterface(const amora::ImGuiEngineData& data, const IconManager& iconManager);

        void layout(OpenMaVisApplication* app);

    private:
        const IconManager& mIconManager;

        MainBar mMainBar;
        SideBar mSideBar;
    };
}

#endif //OPENMAVIS_USERINTERFACE_HPP
