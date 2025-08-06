#pragma once

#include <imgui.h>

#include "service/ProcessUpdater.h"


struct Column {
    char *name;
    float width;
    ImGuiTableColumnFlags flags;
    ProcessInfo::SortField sort_field;
};

class ProcessUI {
    public:
        ProcessUI();
        ~ProcessUI();
        void render();
    private:
        std::unique_ptr<ProcessUpdater> m_ProcessUpdater;
        std::vector<ProcessInfo> m_DisplayedProcesses;
};
