#include "ProcessUI.h"

#include <algorithm>
#include <format>
#include <iostream>

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


std::vector<Column> extractColumns(const std::vector<ProcessInfo> &processes) {
    float padding = ImGui::GetStyle().FramePadding.x * 2;

    std::array<float, 7> widths = {0};
    for (const auto &process: processes) {
        widths[0] = std::max(widths[0], ImGui::CalcTextSize(std::to_string(process.pid).c_str()).x + padding);
        widths[1] = std::max(widths[1], ImGui::CalcTextSize(process.name.c_str()).x + padding);
        widths[2] = std::max(widths[2], ImGui::CalcTextSize(process.user.c_str()).x + padding);
        widths[3] = std::max(widths[3], ImGui::CalcTextSize(std::format("{:.2f}", process.cpu_usage).c_str()).x + padding);
        widths[4] = std::max(widths[4], ImGui::CalcTextSize(std::format("{:.2f}", process.memory_usage / 1024.0f).c_str()).x + padding);
        widths[5] = std::max(widths[5], ImGui::CalcTextSize(std::format("{:.2f}", process.memory_percent).c_str()).x + padding);
        widths[6] = std::max(widths[6], ImGui::CalcTextSize(process.command.c_str()).x);
    }

    std::vector<Column> columns;

    char *pid = (char *) "(PID)";
    const float pid_width = std::max(widths[0], ImGui::CalcTextSize(pid).x + padding);
    columns.push_back(Column{pid, pid_width, ImGuiTableColumnFlags_WidthFixed,ProcessInfo::PID});

    char *name = (char *) "Name";
    const float name_width = std::max(widths[1], ImGui::CalcTextSize(name).x + padding);
    columns.push_back(Column{name, name_width, ImGuiTableColumnFlags_WidthFixed,ProcessInfo::NAME});

    char *user = (char *) "User";
    const float user_width = std::max(widths[2], ImGui::CalcTextSize(user).x + padding);
    columns.push_back(Column{user, user_width, ImGuiTableColumnFlags_WidthFixed,ProcessInfo::USER});

    char *cpu = (char *) "CPU (%)";
    const float cpu_width = std::max(widths[3], ImGui::CalcTextSize(cpu).x + padding);
    columns.push_back(Column{cpu, cpu_width, ImGuiTableColumnFlags_WidthFixed,ProcessInfo::CPU});

    char *mem = (char *) "Memory (MB)";
    const float mem_width = std::max(widths[4], ImGui::CalcTextSize(mem).x + padding);
    columns.push_back(Column{mem, mem_width, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_DefaultSort,ProcessInfo::MEMORY});

    char *mem_perc = (char *) "Memory (%)";
    const float mem_perc_width = std::max(widths[5], ImGui::CalcTextSize(mem_perc).x + padding);
    columns.push_back(Column{mem_perc, mem_perc_width, ImGuiTableColumnFlags_WidthFixed,ProcessInfo::MEMORY});

    char *command = (char *) "Command";
    const float command_width = std::max(widths[6], ImGui::CalcTextSize(command).x + padding);
    columns.push_back(Column{command, command_width, ImGuiTableColumnFlags_WidthStretch,ProcessInfo::COMMAND});

    return columns;
}


void renderColumns(const std::vector<ProcessInfo> &processes) {
    for (Column col : extractColumns(processes)) {
        ImGui::TableSetupColumn(col.name, col.flags, col.width, col.sort_field);
    }

    ImGui::TableHeadersRow();
}


void applySortOnProcesses(std::vector<ProcessInfo> *m_DisplayedProcesses) {
    if (ImGuiTableSortSpecs *sort_specs = ImGui::TableGetSortSpecs()) {
        if (sort_specs->SpecsDirty && sort_specs->SpecsCount > 0) {
            const ImGuiTableColumnSortSpecs &spec = sort_specs->Specs[0];
            std::sort(m_DisplayedProcesses->begin(), m_DisplayedProcesses->end(),
                      [&](const ProcessInfo &a, const ProcessInfo &b) {
                          switch (spec.ColumnUserID) {
                              case ProcessInfo::PID: return spec.SortDirection == ImGuiSortDirection_Ascending
                                                                ? a.pid < b.pid
                                                                : a.pid > b.pid;
                              case ProcessInfo::NAME: return spec.SortDirection == ImGuiSortDirection_Ascending
                                                                 ? a.name < b.name
                                                                 : a.name > b.name;
                              case ProcessInfo::USER: return spec.SortDirection == ImGuiSortDirection_Ascending
                                                                 ? a.user < b.user
                                                                 : a.user > b.user;
                              case ProcessInfo::CPU: return spec.SortDirection == ImGuiSortDirection_Ascending
                                                                ? a.cpu_usage < b.cpu_usage
                                                                : a.cpu_usage > b.cpu_usage;
                              case ProcessInfo::MEMORY: return spec.SortDirection == ImGuiSortDirection_Ascending
                                                                   ? a.memory_usage < b.memory_usage
                                                                   : a.memory_usage > b.memory_usage;
                              case ProcessInfo::COMMAND: return spec.SortDirection == ImGuiSortDirection_Ascending
                                                                    ? a.command < b.command
                                                                    : a.command > b.command;
                              default: return false;
                          }
                      });
        }
    }
}

void renderRows(std::vector<ProcessInfo> m_DisplayedProcesses) {
    ImGuiListClipper clipper;
    clipper.Begin(m_DisplayedProcesses.size());
    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
            auto process = m_DisplayedProcesses[row];
            ImGui::PushID(process.pid);
            ImGui::TableNextRow();

            // PID
            ImGui::TableSetColumnIndex(0);

            // Make row selectable
            ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
            if (ImGui::Selectable( std::to_string(process.pid).c_str(), false, selectable_flags, ImVec2(0, 15))) {
                // Simple left click
            }
            if (ImGui::IsItemHovered() &&ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                // Simple right click
                if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
                {
                    ImGui::TextUnformatted("This a popup !");
                    if (ImGui::Button("Close"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
            }
            // Name
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(process.name.c_str());
            // User
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(process.user.c_str());

            // Cpu
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.2f", process.cpu_usage);

            // Memory MB
            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%.2f", process.memory_usage / 1024.0f);

            // Memory %
            ImGui::TableSetColumnIndex(5);
            ImGui::Text("%.2f", process.memory_percent);

            // Command
            ImGui::TableSetColumnIndex(6);
            ImGui::TextUnformatted(process.command.c_str());

            // Tooltip on command cell
            if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 75);
                ImGui::TextUnformatted(process.command.c_str());
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }

            ImGui::PopID();
        }
    }
}

void ProcessUI::render() {
    // Fetch processes infos
    m_DisplayedProcesses = m_ProcessUpdater->get_processes();

    // Tables Flags
    const ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable |
                                  ImGuiTableFlags_SizingStretchProp;

    if (!m_DisplayedProcesses.empty()) {
        if (ImGui::BeginTable("ProcessTable", 7, flags)) {
            renderColumns(m_DisplayedProcesses);
            applySortOnProcesses(&m_DisplayedProcesses);
            renderRows(m_DisplayedProcesses);
            ImGui::EndTable();
        }
    }else {
        ImGui::Text("No processes found");
    }
}
