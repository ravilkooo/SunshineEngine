#pragma once
/*
 * EDITOR USAGE EXAMPLES: PlayerObject_Info Lua Configuration
 * 
 * This file demonstrates how to use PlayerObject_Info in the editor
 * to configure Lua script bindings for player input handling.
 */
/*
#include <PlayerObject/PlayerObject.h>
#include <Utils/AssetPath.h>

// ============================================================================
// EXAMPLE 1: Basic Setup - Create Player and Set Lua Script
// ============================================================================

void Example_BasicSetup()
{
    // Get or create a PlayerObject_Info in the editor
    PlayerObject_Info playerInfo(renderer);

    // Set the Lua script path
    AssetPath scriptPath(L"player_controller.lua", AssetPath::AssetSource::Project);
    playerInfo.SetLuaScriptPath(scriptPath);

    // Verify script is set
    if (playerInfo.HasLuaScript()) {
        const auto& path = playerInfo.GetLuaScriptPath();
        // Display path in editor UI
    }
}

// ============================================================================
// EXAMPLE 2: Add Key-Function Pairs
// ============================================================================

void Example_AddPairs()
{
    PlayerObject_Info playerInfo;

    // Add individual pairs (check for duplicates)
    if (playerInfo.FindPairIndexByKey(Keys::W) == -1) {
        playerInfo.AddKeyFunctionPair(Keys::W, "onMoveForward");
    }

    if (playerInfo.FindPairIndexByKey(Keys::Space) == -1) {
        playerInfo.AddKeyFunctionPair(Keys::Space, "onJump");
    }

    if (playerInfo.FindPairIndexByKey(Keys::E) == -1) {
        playerInfo.AddKeyFunctionPair(Keys::E, "onInteract");
    }

    // Add using KeyFunctionPair struct
    KeyFunctionPair qPair(Keys::Q, "onSpecialAbility");
    playerInfo.AddKeyFunctionPair(qPair);
}

// ============================================================================
// EXAMPLE 3: Edit Existing Pairs
// ============================================================================

void Example_EditPairs()
{
    PlayerObject_Info playerInfo;

    // First, find a pair to edit
    int wIndex = playerInfo.FindPairIndexByKey(Keys::W);
    if (wIndex != -1) {
        // Edit both key and function
        playerInfo.EditKeyFunctionPair(wIndex, Keys::W, "onMoveForwardModified");

        // Or edit just the key
        playerInfo.EditKeyFunctionPairKey(wIndex, Keys::LeftControl);

        // Or edit just the function name
        playerInfo.EditKeyFunctionPairFunction(wIndex, "onNewFunction");
    }
}

// ============================================================================
// EXAMPLE 4: Delete Pairs
// ============================================================================

void Example_DeletePairs()
{
    PlayerObject_Info playerInfo;

    // Find a pair by key and remove it
    int index = playerInfo.FindPairIndexByKey(Keys::E);
    if (index != -1) {
        playerInfo.RemoveKeyFunctionPair(index);
    }

    // Clear all pairs
    playerInfo.ClearKeyFunctionPairs();
}

// ============================================================================
// EXAMPLE 5: Query and Display All Pairs
// ============================================================================

void Example_DisplayAllPairs()
{

    size_t count = playerInfo.GetKeyFunctionPairCount();
    if (count == 0) {
        // No pairs configured
        return;
    }

    // Display in UI table/list
    const auto& pairs = playerInfo.GetAllKeyFunctionPairs();
    for (size_t i = 0; i < pairs.size(); ++i) {
        const auto& pair = pairs[i];
        
        // Display: [Index] Key -> FunctionName
        // e.g.: [0] W -> onMoveForward
        //       [1] Space -> onJump
        //       [2] E -> onInteract
        
        eastl::string display = 
            eastl::to_string(i) + " | " + 
            KeyFunctionPair::KeyToString(pair.key) + " -> " + 
            pair.functionName;
    }
}

// ============================================================================
// EXAMPLE 6: Editor UI Pattern - Add New Binding Dialog
// ============================================================================

void Example_EditorUIAddBinding(PlayerObject_Info& playerInfo)
{
    // Editor UI state
    static Keys selectedKey = Keys::None;
    static eastl::string selectedFunction;

    // UI Step 1: Select Key Enum
    // Dropdown menu with all Keys enum values
    // When user selects a key: selectedKey = Keys::W
    
    // UI Step 2: Select Function Name
    // Dropdown populated from Lua script's available functions
    // When user selects function: selectedFunction = "onMoveForward"

    // UI Step 3: Validate and Add
    if (selectedKey != Keys::None && !selectedFunction.empty()) {
        // Check for duplicate keys
        if (playerInfo.FindPairIndexByKey(selectedKey) == -1) {
            playerInfo.AddKeyFunctionPair(selectedKey, selectedFunction);
            // Reset UI
            selectedKey = Keys::None;
            selectedFunction.clear();
        } else {
            // Show error: "Key already bound to another function"
        }
    }
}

// ============================================================================
// EXAMPLE 7: Editor UI Pattern - Edit Existing Binding
// ============================================================================

void Example_EditorUIEditBinding(PlayerObject_Info& playerInfo, size_t pairIndex)
{
    // Get the pair to edit
    const auto* pair = playerInfo.GetKeyFunctionPair(pairIndex);
    if (!pair) return;

    // Editor UI state
    static Keys newKey = Keys::None;
    static eastl::string newFunction;

    // UI Step 1: Display current binding
    // Show: Current Key = "W", Current Function = "onMoveForward"

    // UI Step 2: Allow user to change key
    // Dropdown menu with all Keys enum values
    
    // UI Step 3: Allow user to change function
    // Dropdown populated from Lua script's available functions

    // UI Step 4: Validate and Update
    if (newKey != Keys::None && !newFunction.empty()) {
        // Check for duplicate keys (excluding current pair)
        int duplicateIndex = playerInfo.FindPairIndexByKey(newKey);
        if (duplicateIndex == -1 || duplicateIndex == (int)pairIndex) {
            playerInfo.EditKeyFunctionPair(pairIndex, newKey, newFunction);
        } else {
            // Show error: "Key already bound"
        }
    }
}

// ============================================================================
// EXAMPLE 8: Editor UI Pattern - Delete Binding
// ============================================================================

void Example_EditorUIDeleteBinding(PlayerObject_Info& playerInfo, size_t pairIndex)
{
    // Show confirmation dialog
    // "Are you sure you want to delete this binding?"
    
    if (false) {
        playerInfo.RemoveKeyFunctionPair(pairIndex);
    }
}

// ============================================================================
// EXAMPLE 9: JSON Serialization (Save/Load)
// ============================================================================

void Example_Serialization()
{
    PlayerObject_Info playerInfo;
    
    // Setup some bindings
    playerInfo.SetLuaScriptPath(
        AssetPath(L"player_controller.lua", AssetPath::AssetSource::Project));
    playerInfo.AddKeyFunctionPair(Keys::W, "onMoveForward");
    playerInfo.AddKeyFunctionPair(Keys::Space, "onJump");

    // Save to JSON
    json configJson = playerInfo.SettingsToJson();
    // Will produce:
    // {
    //   "luaScript": "Projects/Templates/player_controller.lua",
    //   "keyFunctionMappings": [
    //     {"key": 87, "functionName": "onMoveForward"},
    //     {"key": 32, "functionName": "onJump"}
    //   ]
    // }

    // Load from JSON
    PlayerObject_Info loadedInfo;
    loadedInfo.SettingsFromJson(configJson, renderer);
    
    // Verify loaded data
    assert(loadedInfo.GetKeyFunctionPairCount() == 2);
    assert(loadedInfo.HasLuaScript());
}

// ============================================================================
// EXAMPLE 10: Complete Editor Panel Pattern
// ============================================================================

class PlayerObjectEditorPanel
{
private:
    PlayerObject_Info* m_playerInfo;
    size_t m_selectedPairIndex = -1;
    
    // Dialog states
    bool m_showAddDialog = false;
    bool m_showEditDialog = false;
    Keys m_dialogSelectedKey = Keys::None;
    eastl::string m_dialogSelectedFunction;

public:
    void DrawUI()
    {
        // ===== Lua Script Selection =====
        ImGui::Text("Lua Script:");
        ImGui::SameLine();
        
        if (m_playerInfo->HasLuaScript()) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "✓ Set");
        } else {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "✗ Not Set");
        }

        ImGui::SameLine();
        if (ImGui::Button("Select Script")) {
            // Open file browser dialog
        }

        if (m_playerInfo->HasLuaScript()) {
            ImGui::Text("  Path: %s", 
                WStringToUtf8(m_playerInfo->GetLuaScriptPath().GetFullPath()).c_str());
        }

        ImGui::Separator();

        // ===== Key-Function Mappings List =====
        ImGui::Text("Key Bindings (%zu):", m_playerInfo->GetKeyFunctionPairCount());

        if (ImGui::BeginTable("KeyBindings", 3, ImGuiTableFlags_Borders)) {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableHeadersRow();

            const auto& pairs = m_playerInfo->GetAllKeyFunctionPairs();
            for (size_t i = 0; i < pairs.size(); ++i) {
                const auto& pair = pairs[i];
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", KeyFunctionPair::KeyToString(pair.key).c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", pair.functionName.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::PushID(static_cast<int>(i));
                
                if (ImGui::SmallButton("Edit")) {
                    m_selectedPairIndex = i;
                    m_showEditDialog = true;
                    m_dialogSelectedKey = pair.key;
                    m_dialogSelectedFunction = pair.functionName;
                }
                
                ImGui::SameLine();
                
                if (ImGui::SmallButton("Delete")) {
                    m_playerInfo->RemoveKeyFunctionPair(i);
                }
                
                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        ImGui::Separator();

        // ===== Add New Binding Button =====
        if (ImGui::Button("+ Add Binding", ImVec2(150, 0))) {
            m_showAddDialog = true;
            m_dialogSelectedKey = Keys::None;
            m_dialogSelectedFunction.clear();
        }

        // ===== Dialogs =====
        if (m_showAddDialog) {
            DrawAddDialog();
        }
        if (m_showEditDialog) {
            DrawEditDialog();
        }
    }

private:
    void DrawAddDialog()
    {
        ImGui::OpenPopup("Add Key Binding");
        
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Add Key Binding", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Select a key and function name:");
            ImGui::Separator();

            // Key selection dropdown
            const char* keyNames[] = { "W", "A", "S", "D", "Space", "E", "Q", "Shift", "Ctrl" };
            Keys keyValues[] = { Keys::W, Keys::A, Keys::S, Keys::D, Keys::Space, 
                               Keys::E, Keys::Q, Keys::ShiftKey, Keys::ControlKey };
            
            static int selectedKeyIdx = 0;
            ImGui::Combo("Key##add", &selectedKeyIdx, keyNames, IM_ARRAYSIZE(keyNames));
            m_dialogSelectedKey = keyValues[selectedKeyIdx];

            // Function name input or dropdown
            char functionBuf[128];
            strcpy_s(functionBuf, m_dialogSelectedFunction.c_str());
            ImGui::InputText("Function Name##add", functionBuf, sizeof(functionBuf));
            m_dialogSelectedFunction = functionBuf;

            ImGui::Separator();

            if (ImGui::Button("Add", ImVec2(120, 0))) {
                if (m_dialogSelectedKey != Keys::None && !m_dialogSelectedFunction.empty()) {
                    if (m_playerInfo->FindPairIndexByKey(m_dialogSelectedKey) == -1) {
                        m_playerInfo->AddKeyFunctionPair(m_dialogSelectedKey, m_dialogSelectedFunction);
                        m_showAddDialog = false;
                        ImGui::CloseCurrentPopup();
                    }
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                m_showAddDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void DrawEditDialog()
    {
        ImGui::OpenPopup("Edit Key Binding");
        
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Edit Key Binding", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Edit key and function:");
            ImGui::Separator();

            // Key selection dropdown
            const char* keyNames[] = { "W", "A", "S", "D", "Space", "E", "Q", "Shift", "Ctrl" };
            Keys keyValues[] = { Keys::W, Keys::A, Keys::S, Keys::D, Keys::Space, 
                               Keys::E, Keys::Q, Keys::ShiftKey, Keys::ControlKey };
            
            static int selectedKeyIdx = 0;
            ImGui::Combo("Key##edit", &selectedKeyIdx, keyNames, IM_ARRAYSIZE(keyNames));
            m_dialogSelectedKey = keyValues[selectedKeyIdx];

            // Function name input
            char functionBuf[128];
            strcpy_s(functionBuf, m_dialogSelectedFunction.c_str());
            ImGui::InputText("Function Name##edit", functionBuf, sizeof(functionBuf));
            m_dialogSelectedFunction = functionBuf;

            ImGui::Separator();

            if (ImGui::Button("Update", ImVec2(120, 0))) {
                if (m_playerInfo->EditKeyFunctionPair(
                    m_selectedPairIndex, m_dialogSelectedKey, m_dialogSelectedFunction)) {
                    m_showEditDialog = false;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                m_showEditDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
};

// ============================================================================
// USAGE: In your ImGui editor panel
// ============================================================================

void RenderPlayerObjectPanel(PlayerObject_Info& playerInfo)
{
    static PlayerObjectEditorPanel panel;
    panel.m_playerInfo = &playerInfo;
    panel.DrawUI();
}
*/
