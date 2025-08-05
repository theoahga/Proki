#pragma once
#include <functional>

#include "process/ProcessUI.h"

struct UiState;  // Forward declaration for use in std::function

struct Tab {
    const char* id;
    bool is_open;
    std::function<void(UiState&)> render;
};

struct UiState {
    int table_index = 0;
    std::vector<Tab> tabs;
    std::unique_ptr<ProcessUI> process_ui = std::make_unique<ProcessUI>();
};

class UI {
public:
    UI();
    ~UI();
    void render();

private:
    UiState* state = new UiState();
    void init();

    void renderMainWindow();
    void renderMenu();
    void renderTabs();
    void renderContent();
};
