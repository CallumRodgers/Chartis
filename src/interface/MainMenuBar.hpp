//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_MAINMENUBAR_HPP
#define OPENMAVIS_MAINMENUBAR_HPP

#include <imgui.h>

namespace mavis::interface {
    class MainMenuBar {
    public:
        static void create() {
            if (ImGui::BeginMainMenuBar()) {

                if (ImGui::BeginMenu("Scene")) {

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Edit")) {

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Export")) {

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help")) {

                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }
        }
    };
}

#endif //OPENMAVIS_MAINMENUBAR_HPP
