#include "UI/LogPanel.h"
#include <EditorLogManager.h>
#include <Utils/GameLogManager.h>

LogPanel::LogPanel(const char* title, ILogManager::LogTarget target) : m_Title(title), m_Target(target) {}

void LogPanel::OnImguiRender(bool& showLogPanel)
{
    if (!showLogPanel) return;
        
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImVec2 minSize(displaySize.x, displaySize.y * 0.4f); 
    ImVec2 maxSize(displaySize.x, displaySize.y - m_BottomOffset);
        
    ImGui::SetNextWindowSizeConstraints(minSize, maxSize, nullptr);
    ImGui::SetNextWindowPos(ImVec2(0.0f, displaySize.y - m_BottomOffset - m_CurrentHeight), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, m_CurrentHeight), ImGuiCond_Always);
        
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing;

    if (!ImGui::Begin(m_Title, &showLogPanel, flags))
    {
        ImGui::End();
        return;
    }
    m_CurrentHeight = ImGui::GetWindowHeight();

    if (ImGui::Button("Clear"))
    {
        if (m_Target == ILogManager::LogTarget::Game)
            GameLogManager::Get().Clear();
        else
            EditorLogManager::Get().Clear();
    }
            

    ImGui::SameLine();
        
    bool copy = ImGui::Button("Copy");
        
    ImGui::SameLine();

    m_Filter.Draw("Filter", -100.0f);
        
    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        auto logs = (m_Target == ILogManager::LogTarget::Game) ?
            GameLogManager::Get().GetLogs() : EditorLogManager::Get().GetLogs();

        for (auto& entry : logs)
        {
            if (!m_Filter.PassFilter(entry.message.c_str()))
                continue;

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(entry.color[0], entry.color[1], entry.color[2], entry.color[3]));
            ImGui::TextUnformatted(entry.message.c_str());
            ImGui::PopStyleColor();
        }

        if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();

    if (ImGui::IsMouseClicked(0)
    && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)
    && ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    {
        showLogPanel = false;
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void LogPanel::SetBottomOffset(float bottomOffset)
{
    m_BottomOffset = bottomOffset;
}