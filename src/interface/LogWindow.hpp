//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_LOGWINDOW_HPP
#define CHARTIS_LOGWINDOW_HPP
#include "imgui.h"

namespace chartis::interface {
    class LogWindow {
    public:
        void layout() {
            if (ImGui::Begin(("Log"))) {
                ImGui::BeginChild("LogScrollableRegion", ImVec2(0.f, 300.f));
                ImGui::TextUnformatted("This is some log");
                ImGui::EndChild();
            }
            ImGui::End();
        }
    };
};

#endif //CHARTIS_LOGWINDOW_HPP
