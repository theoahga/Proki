#pragma once

#include "service/ProcessUpdater.h"


class ProcessUI {
    public:
        ProcessUI();
        ~ProcessUI();
        void render();
    private:
        std::unique_ptr<ProcessUpdater> m_ProcessUpdater;
        std::vector<ProcessInfo> m_DisplayedProcesses;
};
