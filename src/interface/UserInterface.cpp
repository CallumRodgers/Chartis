//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "UserInterface.hpp"

#include <imgui.h>
#include <stdexcept>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include "MainMenuBar.hpp"
#include "TabBar.hpp"

using namespace mavis::interface;

void UserInterface::setup(const amora::ImGuiEngineData& data) {
    ImGui::SetAllocatorFunctions(data.AllocFunc, data.FreeFunc, data.AllocUserData);
    ImGui::SetCurrentContext(data.Context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
    if (ImGui::Begin("OpenMaVis", nullptr, flags)) {
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

        MainMenuBar::create();
        TabBar::create();

        ImGui::End();
    } else {
        throw std::runtime_error("Could not create main ImGui window!");
    }
}
