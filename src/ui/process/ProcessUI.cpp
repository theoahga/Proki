#include "ProcessUI.h"
#include "imgui.h"
#include "service/ProcessService.h"

ProcessUI::ProcessUI() {
    shared_ptr_var = std::make_shared<std::vector<ProcessInfo>>();
    process_updater = new ProcessUpdater(&shared_ptr_var);
    process_updater->start();
}

ProcessUI::~ProcessUI() {
    process_updater->stop();
    delete process_updater;
}

void ProcessUI::render() {
    std::shared_ptr<std::vector<ProcessInfo>> current_process_list_ptr = std::atomic_load(&shared_ptr_var);

    if (!current_process_list_ptr) {
        ImGui::Text("Loading processes...");
        return;
    }

    if (ImGui::BeginChild("process_monitor")) {
        if (ImGui::BeginTable("ProcessTable", 6,
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
                              ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("PID");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("User");
            ImGui::TableSetupColumn("Memory (MB)");
            ImGui::TableSetupColumn("CPU (%)");
            ImGui::TableSetupColumn("Threads");
            ImGui::TableHeadersRow();


            for (ProcessInfo &process_info: *current_process_list_ptr) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", process_info.pid);

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(process_info.name.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(process_info.user.c_str());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2f", process_info.memory_usage);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.2f", process_info.cpu_usage);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%d", process_info.thread_count);
            }

            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}
