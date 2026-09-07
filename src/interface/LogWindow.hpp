////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

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
