#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include <UI/PlayerSettingPanel.h>
#include <UI/PropertyPanel.h>
#include <UI/FontStyles.h>

#include <Graphics/Utils/Camera.h>
#include <Graphics/Renderer/GBuffer.h>

#include <PlayerObject/PlayerObject.h>
#include <Utils/StringUtils.h>

void PlayerSettingPanel::OnImGuiRender()
{
    DrawPlayerObjectDetails();
}

void PlayerSettingPanel::SetPlayerObject(PlayerObject_Info* playerObj)
{
    m_playerObject = playerObj;
};

void PlayerSettingPanel::DrawPlayerObjectDetails()
{
    DrawPlayerCameraDetails();
    DrawPlayerControllerDetails();
}

void PlayerSettingPanel::DrawPlayerCameraDetails()
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header1);
    
    if (ImGui::TreeNodeEx("Camera Component", flags))
    {
        EditorUI::FontStyles::Pop();

        /*
        ImVec2 avail = ImGui::GetContentRegionAvail();
        avail.y = avail.x * 360.0f / 640.0f;

        // ImGui::Image((ImTextureID) m_playerObject->m_miniViewRenderer->m_GBuffer->pLightSRV.Get(), avail);

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Spring Arm Params");
        EditorUI::FontStyles::Pop();

        float stickLength = m_playerObject->m_playerCamera->m_springArmParams.length;
        if (ImGui::DragFloat("Length", &stickLength, 0.1f, 0.1f, 90.0f, "%.1f m"))
        {
            m_playerObject->m_playerCamera->m_springArmParams.length = stickLength;
        }

        DXSM::Vector3 springArmRotationDeg = m_playerObject->m_playerCamera->m_springArmParams.pitchYawRoll * (180.0f / DirectX::XM_PI);
        if (PropertyPanel::DrawVector3Control("Rotation", springArmRotationDeg,
            DXSM::Vector3(-90.0f, -80.0f, -360.0f),
            DXSM::Vector3(90.0f, 80.0f, 360.0f),
            0.0f))
        {
            m_playerObject->m_playerCamera->m_springArmParams.pitchYawRoll = springArmRotationDeg * (DirectX::XM_PI / 180.0f);
        }

        PropertyPanel::DrawVector3Control("Offset",
            m_playerObject->m_playerCamera->m_springArmParams.rootOffset,
            DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
            DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
            0.0f);

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Camera Params");
        EditorUI::FontStyles::Pop();

        DXSM::Vector3 cameraRotationDeg = m_playerObject->m_playerCamera->cameraPitchYawRoll * (180.0f / DirectX::XM_PI);
        if (PropertyPanel::DrawVector3Control("Rotation", cameraRotationDeg,
            DXSM::Vector3(-90.0f, -80.0f, -360.0f),
            DXSM::Vector3(90.0f, 80.0f, 360.0f),
            0.0f))
        {
            m_playerObject->m_playerCamera->cameraPitchYawRoll = cameraRotationDeg * (DirectX::XM_PI / 180.0f);
        }

        */
        ImGui::TreePop();
    }
    else
        EditorUI::FontStyles::Pop();

	return;
}

void PlayerSettingPanel::DrawPlayerControllerDetails()
{
    static int selectedKey = 0;
    static bool keyUpdated = false;
    static eastl::vector<Keys> availableKeys;
    static eastl::vector<eastl::string> keyNames;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header1);

    if (ImGui::TreeNodeEx("Controller Settings", flags))
    {
        EditorUI::FontStyles::Pop();

        if (!m_playerObject) {
            ImGui::TextDisabled("No player object selected");
            ImGui::TreePop();
            return;
        }

        // ===== LUA SCRIPT SELECTION =====
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Lua Script");
        EditorUI::FontStyles::Pop();

        const AssetPath& currentScript = m_playerObject->GetLuaScriptPath();
        eastl::string scriptPath = WStringToUtf8(currentScript.m_assetRelativePath);
        
        ImGui::SameLine();
        if (m_playerObject->HasLuaScript())
        {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%.50s", scriptPath.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "[Not Set]");
        }

        ImGui::SameLine();
        if (ImGui::Button("Select Script##LuaScript", ImVec2(120, 0))) {
            // TODO: Open file browser for .lua files in $current_project_path$/ directory
            // For now, placeholder: user can type path manually
            ImGui::OpenPopup("SelectLuaScriptPopup");
        }

        // Simple text input popup for script path (placeholder for file browser)
        if (ImGui::BeginPopupModal("SelectLuaScriptPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char scriptBuffer[512] = "";
            ImGui::InputText("Lua Script Path##Input", scriptBuffer, IM_ARRAYSIZE(scriptBuffer));
            ImGui::TextDisabled("(e.g., Scripts/player_controller.lua)");

            if (ImGui::Button("Confirm##LuaScript", ImVec2(100, 0))) {
                if (scriptBuffer[0] != '\0') {
                    eastl::string utf8Path(scriptBuffer);
                    AssetPath newPath(Utf8ToWString(utf8Path), AssetPath::AssetSource::Project);
                    m_playerObject->SetLuaScriptPath(newPath);
                    scriptBuffer[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Clear##LuaScript", ImVec2(100, 0))) {
                m_playerObject->SetLuaScriptPath(AssetPath());
                scriptBuffer[0] = '\0';
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel##LuaScript", ImVec2(100, 0))) {
                scriptBuffer[0] = '\0';
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ===== MOUSE ACTIONS CONFIGURATION =====
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Mouse Actions");
        EditorUI::FontStyles::Pop();

        ImGui::Checkbox("Fixed camera", &m_playerObject->m_fixedCamera);
        ImGui::SetItemTooltip(
            "Turn on/off default camera\n"
            "movement controls"
        );

        ImGui::Text("Mouse Handler Function:");
        ImGui::SameLine();

        static char mouseHandlerBuffer[256] = "";
        // Initialize buffer with current value
        static bool mouseBufferInitialized = false;
        if (!mouseBufferInitialized && m_playerObject) {
            eastl::string currentHandler = m_playerObject->m_luaActionMapping.GetMouseActionsHandlingFunction();
            if (!currentHandler.empty()) {
                strncpy_s(mouseHandlerBuffer, currentHandler.c_str(), sizeof(mouseHandlerBuffer) - 1);
            }
            else {
                strncpy_s(mouseHandlerBuffer, "onLookAround", sizeof(mouseHandlerBuffer) - 1);
            }
            mouseBufferInitialized = true;
        }

        float total = ImGui::GetContentRegionAvail().x;
        ImGui::SetNextItemWidth(total * 0.7f);
        ImGui::InputText("##MouseHandler", mouseHandlerBuffer, IM_ARRAYSIZE(mouseHandlerBuffer));
        ImGui::SameLine();

        if (ImGui::Button("Set##MouseHandler", ImVec2(0, 0))) {
            if (mouseHandlerBuffer[0] != '\0' && m_playerObject) {
                // Validate function exists in Lua
                sol::state* luaState = m_playerObject->m_luaActionMapping.GetLuaState();
                if (luaState) {
                    sol::function func = (*luaState)[mouseHandlerBuffer];
                    if (func.valid()) {
                        m_playerObject->m_luaActionMapping.SetMouseActionsHandlingFunction(
                            eastl::string(mouseHandlerBuffer));
                        ImGui::OpenPopup("MouseHandlerSetPopup");
                    }
                    else {
                        ImGui::OpenPopup("MouseHandlerErrorPopup");
                    }
                }
            }
        }

        ImGui::TextDisabled("(function signature: function(deltaX, deltaY, wheelDelta))");

        // Success message
        if (ImGui::BeginPopupModal("MouseHandlerSetPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Mouse handler set successfully!");
            ImGui::Text("Function: %s", mouseHandlerBuffer);
            if (ImGui::Button("OK##MouseHandlerSet", ImVec2(100, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // Error message
        if (ImGui::BeginPopupModal("MouseHandlerErrorPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error: Function not found!");
            ImGui::Text("Function '%s' not found in Lua script.", mouseHandlerBuffer);
            ImGui::TextDisabled("Make sure the function is defined in your Lua script.");
            if (ImGui::Button("OK##MouseHandlerError", ImVec2(100, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ===== KEY-FUNCTION PAIR MANAGEMENT =====
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Key Bindings");
        EditorUI::FontStyles::Pop();

        size_t pairCount = m_playerObject->GetKeyFunctionPairCount();

        // Add new pair button
        if (ImGui::Button("+ Add Binding", ImVec2(120, 0))) {
            ImGui::OpenPopup("AddBindingPopup");
        }

        ImGui::SameLine();
        if (pairCount > 0 && ImGui::Button("Clear All", ImVec2(100, 0))) {
            if (ImGui::BeginPopupModal("ConfirmClearAllPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Clear all key bindings?");
                if (ImGui::Button("Yes##ClearAll", ImVec2(80, 0))) {
                    m_playerObject->ClearKeyFunctionPairs();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("No##ClearAll", ImVec2(80, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        ImGui::Spacing();

        // Display all bindings in a table
        if (pairCount > 0) {
            if (ImGui::BeginTable("KeyBindingsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableHeadersRow();

                const auto& allPairs = m_playerObject->GetAllKeyFunctionPairs();
                for (size_t i = 0; i < allPairs.size(); ++i) {
                    const auto& pair = allPairs[i];
                    ImGui::TableNextRow();

                    // Key column
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", KeyFunctionPair::KeyToString(pair.key).c_str());

                    // Function name column
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", pair.functionName.c_str());

                    // Edit button
                    ImGui::TableSetColumnIndex(2);
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::Button("Edit", ImVec2(50, 0))) {
                        ImGui::OpenPopup("EditBindingPopup");
                        keyUpdated = false;
						selectedKey = 0; // Reset selected key for edit
                    }

                    // Delete button
                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::Button("Del", ImVec2(50, 0))) {
                        ImGui::OpenPopup("ConfirmDeletePopup");
                    }

                    static int editKeyIndex = -1;
                    // Edit binding modal
                    if (ImGui::BeginPopupModal("EditBindingPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                        if (editKeyIndex == -1) {
                            editKeyIndex = static_cast<int>(i);
                        }

                        const auto& editPair = m_playerObject->GetAllKeyFunctionPairs()[editKeyIndex];

                        ImGui::Text("Edit Key Binding");
                        ImGui::Separator();

                        // Key selector dropdown

                        if (availableKeys.empty()) {
                            // Populate available keys
                            availableKeys = {
                                Keys::W, Keys::A, Keys::S, Keys::D, Keys::Space,
                                Keys::E, Keys::Q, Keys::LeftButton, Keys::RightButton, Keys::MiddleButton,
                                Keys::Escape, Keys::Enter, Keys::Tab, Keys::Delete, Keys::Insert,
                                Keys::F1, Keys::F2, Keys::F3, Keys::F4, Keys::F5, Keys::F6,
                                Keys::F7, Keys::F8, Keys::F9, Keys::F10, Keys::F11, Keys::F12,
                                Keys::LeftControl, Keys::RightControl,
                                Keys::LeftShift, Keys::RightShift,
                                Keys::LeftAlt, Keys::RightAlt,
                                Keys::Up, Keys::Down, Keys::Left, Keys::Right,
                                Keys::Home, Keys::End, Keys::PageUp, Keys::PageDown,
                                Keys::D0, Keys::D1, Keys::D2, Keys::D3, Keys::D4,
                                Keys::D5, Keys::D6, Keys::D7, Keys::D8, Keys::D9
                            };
                            keyNames.clear();
                            for (Keys k : availableKeys) {
                                keyNames.push_back(KeyFunctionPair::KeyToString(k));
                            }
                        }

                        // Find current key index
                        if (!keyUpdated)
                        {
                            for (size_t j = 0; j < availableKeys.size(); ++j) {
                                if (availableKeys[j] == editPair.key) {
                                    selectedKey = static_cast<int>(j);
                                    break;
                                }
                            }
                        }

                        ImGui::Text("Select Key:");
                        const char* preview = selectedKey < keyNames.size() ? keyNames[selectedKey].c_str() : "Unknown";
                        if (ImGui::BeginCombo("##KeyCombo", preview)) {
                            for (int k = 0; k < static_cast<int>(availableKeys.size()); ++k) {
                                bool isSelected = (selectedKey == k);
                                if (ImGui::Selectable(keyNames[k].c_str(), isSelected)) {
                                    selectedKey = k;
                                    keyUpdated = true;
                                }
                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }

                        static char funcNameBuffer[256] = "";
                        if (strlen(funcNameBuffer) == 0) {
                            strncpy_s(funcNameBuffer, editPair.functionName.c_str(), sizeof(funcNameBuffer) - 1);
                        }

                        ImGui::Text("Function Name:");
                        ImGui::InputText("##FunctionName", funcNameBuffer, IM_ARRAYSIZE(funcNameBuffer));

                        ImGui::Spacing();

                        if (ImGui::Button("Save##Edit", ImVec2(100, 0))) {
                            if (selectedKey >= 0 && selectedKey < static_cast<int>(availableKeys.size())) {
                                Keys newKey = availableKeys[selectedKey];
                                eastl::string newFunc(funcNameBuffer);

                                // Check for duplicate keys (excluding current pair)
                                bool isDuplicate = false;
                                int dupIndex = m_playerObject->FindPairIndexByKey(newKey);
                                if (dupIndex >= 0 && dupIndex != editKeyIndex) {
                                    isDuplicate = true;
                                }

                                if (isDuplicate) {
                                    ImGui::OpenPopup("DuplicateKeyWarning");
                                } else if (!newFunc.empty()) {
                                    m_playerObject->EditKeyFunctionPair(editKeyIndex, newKey, newFunc);
                                    editKeyIndex = -1;
                                    selectedKey = 0;
                                    funcNameBuffer[0] = '\0';
                                    ImGui::CloseCurrentPopup();
                                }
                            }
                        }

                        ImGui::SameLine();
                        if (ImGui::Button("Cancel##Edit", ImVec2(100, 0))) {
                            editKeyIndex = -1;
                            selectedKey = 0;
                            funcNameBuffer[0] = '\0';
                            ImGui::CloseCurrentPopup();
                        }

                        // Duplicate key warning
                        if (ImGui::BeginPopupModal("DuplicateKeyWarning", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Key already bound!");
                            ImGui::Text("This key is already assigned to another function.");
                            if (ImGui::Button("OK##DupKey", ImVec2(100, 0))) {
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        }

                        ImGui::EndPopup();
                    }

                    // Delete confirmation modal
                    if (ImGui::BeginPopupModal("ConfirmDeletePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                        if (editKeyIndex == -1) {
                            editKeyIndex = static_cast<int>(i);
                        }

                        const auto& editPair = m_playerObject->GetAllKeyFunctionPairs()[editKeyIndex];
                        
                        ImGui::Text("Delete this binding?");
                        ImGui::Text("Key: %s -> Function: %s", 
                            KeyFunctionPair::KeyToString(editPair.key).c_str(),
                            editPair.functionName.c_str());

                        if (ImGui::Button("Delete##Confirm", ImVec2(100, 0))) {
                            m_playerObject->RemoveKeyFunctionPair(i);
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::SameLine();
                        if (ImGui::Button("Cancel##Delete", ImVec2(100, 0))) {
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::EndPopup();
                    }

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        } else {
            ImGui::TextDisabled("No key bindings configured");
        }

        ImGui::Spacing();

        // Add new binding modal
        if (ImGui::BeginPopupModal("AddBindingPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            static int newKeyIndex = 0;
            static eastl::vector<Keys> availableKeys;
            static eastl::vector<eastl::string> keyNames;
            static char funcNameBuffer[256] = "";

            if (availableKeys.empty()) {
                availableKeys = {
                    Keys::W, Keys::A, Keys::S, Keys::D, Keys::Space,
                    Keys::E, Keys::Q, Keys::LeftButton, Keys::RightButton, Keys::MiddleButton,
                    Keys::Escape, Keys::Enter, Keys::Tab, Keys::Delete, Keys::Insert,
                    Keys::F1, Keys::F2, Keys::F3, Keys::F4, Keys::F5, Keys::F6,
                    Keys::F7, Keys::F8, Keys::F9, Keys::F10, Keys::F11, Keys::F12,
                    Keys::LeftControl, Keys::RightControl, Keys::LeftShift, Keys::RightShift,
                    Keys::LeftAlt, Keys::RightAlt,
                    Keys::Up, Keys::Down, Keys::Left, Keys::Right,
                    Keys::Home, Keys::End, Keys::PageUp, Keys::PageDown,
                    Keys::D0, Keys::D1, Keys::D2, Keys::D3, Keys::D4,
                    Keys::D5, Keys::D6, Keys::D7, Keys::D8, Keys::D9
                };
                keyNames.clear();
                for (Keys k : availableKeys) {
                    keyNames.push_back(KeyFunctionPair::KeyToString(k));
                }
            }

            ImGui::Text("Add New Key Binding");
            ImGui::Separator();

            ImGui::Text("Select Key:");
            const char* preview = newKeyIndex < static_cast<int>(keyNames.size()) ? keyNames[newKeyIndex].c_str() : "Unknown";
            if (ImGui::BeginCombo("##NewKeyCombo", preview)) {
                for (int k = 0; k < static_cast<int>(availableKeys.size()); ++k) {
                    bool isSelected = (newKeyIndex == k);
                    if (ImGui::Selectable(keyNames[k].c_str(), isSelected)) {
                        newKeyIndex = k;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Text("Function Name:");
            ImGui::InputText("##NewFunctionName", funcNameBuffer, IM_ARRAYSIZE(funcNameBuffer));

            ImGui::Spacing();

            if (ImGui::Button("Add##Confirm", ImVec2(100, 0))) {
                if (newKeyIndex >= 0 && newKeyIndex < static_cast<int>(availableKeys.size())) {
                    Keys selectedKey = availableKeys[newKeyIndex];
                    eastl::string funcName(funcNameBuffer);

                    // Check for duplicate keys
                    int dupIndex = m_playerObject->FindPairIndexByKey(selectedKey);
                    if (dupIndex >= 0) {
                        ImGui::OpenPopup("DuplicateKeyWarningAdd");
                    } else if (!funcName.empty()) {
                        m_playerObject->AddKeyFunctionPair(selectedKey, funcName);
                        newKeyIndex = 0;
                        funcNameBuffer[0] = '\0';
                        ImGui::CloseCurrentPopup();
                    }
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel##Add", ImVec2(100, 0))) {
                newKeyIndex = 0;
                funcNameBuffer[0] = '\0';
                ImGui::CloseCurrentPopup();
            }

            // Duplicate key warning for add dialog
            if (ImGui::BeginPopupModal("DuplicateKeyWarningAdd", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Key already bound!");
                ImGui::Text("This key is already assigned to another function.");
                if (ImGui::Button("OK##DupKeyAdd", ImVec2(100, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::TreePop();
    }
    else
        EditorUI::FontStyles::Pop();
}
