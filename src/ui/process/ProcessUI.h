#pragma once

#include "service/ProcessUpdater.h"


class ProcessUI {
    public:
        ProcessUI();
        ~ProcessUI();
        void render();
    private:
        std::shared_ptr<std::vector<ProcessInfo>> shared_ptr_var;
        ProcessUpdater* process_updater = nullptr;
};
