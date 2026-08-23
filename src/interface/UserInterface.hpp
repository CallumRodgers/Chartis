//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_USERINTERFACE_HPP
#define CHARTIS_USERINTERFACE_HPP

#include <AMORA/ImGuiEngineData.hpp>

#include "IconManager.hpp"
#include "MainBar.hpp"
#include "ChartisApplication.hpp"
#include "SideBar.hpp"

namespace chartis::interface {
    class UserInterface {
    public:
        explicit UserInterface(const amora::ImGuiEngineData& data, const IconManager& iconManager);

        void layout(ChartisApplication* app);

    private:
        const IconManager& mIconManager;

        MainBar mMainBar;
        SideBar mSideBar;
    };
}

#endif //CHARTIS_USERINTERFACE_HPP
