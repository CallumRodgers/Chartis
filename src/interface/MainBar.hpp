//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_MAINMENUBAR_HPP
#define OPENMAVIS_MAINMENUBAR_HPP

#include <imgui.h>
#include "OpenMaVisApplication.hpp"
#include "dp.hpp"

namespace mavis::interface {
    class MainBar {
    public:
        void layout(OpenMaVisApplication* app) {

            const float desiredHeight = 48_dp;
            const float fontSize = ImGui::GetFontSize();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, (desiredHeight - fontSize) / 2.f));

            if (ImGui::BeginMainMenuBar()) {

                if (ImGui::Button("=")) {
                    mIsMainMenuShowing = !mIsMainMenuShowing;
                    app->invalidateRender();
                }

                if (mIsMainMenuShowing) {
                    if (ImGui::BeginMenu("OpenMaVis")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Scene")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("View")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Export")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Help")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("About")) {

                        ImGui::EndMenu();
                    }
                } else {
                    if (ImGui::BeginTabBar("SceneTabs")) {

                        if (ImGui::BeginTabItem("Default")) {

                            ImGui::EndTabItem();
                        }

                        if (ImGui::Button("+")) {
                            MAVIS_INFO("NEW TAB!");
                        }

                        ImGui::EndTabBar();
                    }
                }

                ////////////////////
                // WINDOW BUTTONS //
                ////////////////////

                ImGui::SameLine();

                const auto windowButtonsSize = ImVec2(50_dp, 0.f);
                const float windowButtonsPos = ImGui::GetWindowViewport()->WorkSize.x - 3 * windowButtonsSize.x;

                ImGui::SetCursorPosX(windowButtonsPos);

                ImGui::BeginGroup();

                if (!app->mNativeWindow.isFullscreen()) {
                    if (ImGui::Button("Minimise", windowButtonsSize)) {
                        app->mNativeWindow.minimise();
                    }
                }

                if (ImGui::Button("Maximise", windowButtonsSize)) {
                    app->mNativeWindow.toggleMaximise();
                }

                if (ImGui::Button("Close", windowButtonsSize)) {
                    app->mNativeWindow.close();
                }

                ImGui::EndGroup();

                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByPopup)) {
                    if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
                        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                            app->mNativeWindow.drag();
                        } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                            app->mNativeWindow.showNativeWindowMenu();
                        }
                    }
                }

                ImGui::EndMainMenuBar();

                ImGui::PopStyleVar();
            }
        }

    private:
        bool mIsMainMenuShowing = false;
    };
}

#endif //OPENMAVIS_MAINMENUBAR_HPP
