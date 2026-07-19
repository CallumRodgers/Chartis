////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include "UserInterface.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

using namespace mavis;

void UserInterface::setup(const amora::ImGuiEngineData& data) {
    ImGui::SetAllocatorFunctions(data.AllocFunc, data.FreeFunc, data.AllocUserData);
    ImGui::SetCurrentContext(data.Context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    const float scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    style.FontSizeBase = 13.0f;
    ImFont* robotoFont = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf");
    ImGui::PushFont(robotoFont);
}

void UserInterface::layout() {
    ImGui::ShowDemoWindow();


}
