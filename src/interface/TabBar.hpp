//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_TABBAR_HPP
#define OPENMAVIS_TABBAR_HPP

#include "imgui.h"

namespace mavis::interface {
    class TabBar {
    public:
        static void create() {
            if (ImGui::Begin("Tabs")) {
                if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_AutoSelectNewTabs)) {
                    if (ImGui::BeginTabItem("My scene")) {
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("My scene 2")) {
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::End();
        }
    };
};

#endif //OPENMAVIS_TABBAR_HPP
