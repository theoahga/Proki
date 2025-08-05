#include "Ui.h"
#include "imgui.h"
#include "process/ProcessUI.h"

UI::UI() {
    init();
}

UI::~UI() {
    delete state;
    state = nullptr;
}

void UI::init() {
    state->tabs.push_back({
        "Tab1",
        true,
        [](UiState& state) {
            state.process_ui->render();
        }
    });

    state->tabs.push_back({
        "Tab2",
        true,
        [](UiState& state) {
            ImGui::Text("Content of Tab2");
        }
    });
}

void UI::render() {
    renderMainWindow();
}

void UI::renderMainWindow() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    bool p_open = true;

    if (ImGui::Begin("main", &p_open, flags)) {
        renderTabs();
        renderContent();
    }
    ImGui::End();
}

void UI::renderTabs() {
    if (ImGui::BeginTabBar("main_tabs")) {
        for (int i = 0; i < state->tabs.size(); ++i) {
            auto &tab = state->tabs[i];
            if (ImGui::BeginTabItem(tab.id, &tab.is_open)) {
                state->table_index = i;
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

void UI::renderContent() {
    ImGui::BeginChild("content");
    Tab tab = state->tabs[state->table_index];
    tab.render(*state);

    ImGui::EndChild();
}
