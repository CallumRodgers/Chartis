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

                if (ImGui::Button("Minimise", windowButtonsSize)) {
                    glfwIconifyWindow(app->mWindow);
                }


                if (ImGui::Button("Maximise", windowButtonsSize)) {
                    app->toggleFullscreenMode();
                }

                if (ImGui::Button("Close", windowButtonsSize)) {
                    glfwSetWindowShouldClose(app->mWindow, GLFW_TRUE);
                }

                ImGui::EndGroup();

                ImGui::EndMainMenuBar();

                ImGui::PopStyleVar();
            }
        }

    private:
        bool mIsMainMenuShowing = false;
    };
}

#endif //OPENMAVIS_MAINMENUBAR_HPP
