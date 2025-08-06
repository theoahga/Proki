#include "ProcessUI.h"

#include <algorithm>

#include "imgui.h"
#include "service/ProcessService.h"

ProcessUI::ProcessUI() {
    m_ProcessUpdater = std::make_unique<ProcessUpdater>();
    m_ProcessUpdater->start();
}

ProcessUI::~ProcessUI() {
    m_ProcessUpdater->stop();
    delete &m_ProcessUpdater;
}

void ProcessUI::render() {
    // Fetch processes infos
    m_DisplayedProcesses = m_ProcessUpdater->get_processes();

    // Tables Flags
    const ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable;

    if (ImGui::BeginTable("ProcessTable", 7, flags)) {
        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_DefaultSort, 0.0f, ProcessInfo::PID);
        ImGui::TableSetupColumn("Name", 0, 0.0f, ProcessInfo::NAME);
        ImGui::TableSetupColumn("User", 0, 0.0f, ProcessInfo::USER);
        ImGui::TableSetupColumn("CPU (%)", 0, 0.0f, ProcessInfo::CPU);
        ImGui::TableSetupColumn("Memory (MB)", 0, 0.0f, ProcessInfo::MEMORY);
        ImGui::TableSetupColumn("Memory (%)", 0, 0.0f, ProcessInfo::MEMORY);
        ImGui::TableSetupColumn("Threads", 0, 0.0f, ProcessInfo::THREADS);
        ImGui::TableHeadersRow();


        if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
            if (sort_specs->SpecsDirty && sort_specs->SpecsCount > 0) {
                const ImGuiTableColumnSortSpecs& spec = sort_specs->Specs[0];
                std::sort(m_DisplayedProcesses.begin(), m_DisplayedProcesses.end(), [&](const ProcessInfo& a, const ProcessInfo& b) {
                    switch (spec.ColumnUserID) {
                        case ProcessInfo::PID: return spec.SortDirection == ImGuiSortDirection_Ascending ? a.pid < b.pid : a.pid > b.pid;
                        case ProcessInfo::NAME: return spec.SortDirection == ImGuiSortDirection_Ascending ? a.name < b.name : a.name > b.name;
                        case ProcessInfo::USER: return spec.SortDirection == ImGuiSortDirection_Ascending ? a.user < b.user : a.user > b.user;
                        case ProcessInfo::CPU: return spec.SortDirection == ImGuiSortDirection_Ascending ? a.cpu_usage < b.cpu_usage : a.cpu_usage > b.cpu_usage;
                        case ProcessInfo::MEMORY: return spec.SortDirection == ImGuiSortDirection_Ascending ? a.memory_usage < b.memory_usage : a.memory_usage > b.memory_usage;
                        case ProcessInfo::THREADS: return spec.SortDirection == ImGuiSortDirection_Ascending ? a.thread_count < b.thread_count : a.thread_count > b.thread_count;
                        default: return false;
                    }
                });
            }
        }

        // Display all clippers
        ImGuiListClipper clipper;
        clipper.Begin(m_DisplayedProcesses.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                auto &process = m_DisplayedProcesses[row];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", process.pid);
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(process.name.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(process.user.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2f", process.cpu_usage);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.2f", process.memory_usage / 1024.0f);
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%.2f", process.memory_percent);
                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%d", process.thread_count);
            }
        }
        ImGui::EndTable();
    }
}
