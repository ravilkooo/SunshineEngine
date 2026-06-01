#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <UI/PlayerSettingPanel.h>
#include <UI/PropertyPanel.h>
#include <UI/FontStyles.h>
#include <UI/ImguiUtils.h>

#include <Graphics/Utils/Camera.h>
#include <Graphics/Renderer/GBuffer.h>

#include <InputSystem/KeyInfo.h>

#include <CameraManager.h>
#include <WorldEditor.h>
#include <Scene.h>
#include <Graphics/Renderer/MiniViewRenderer.h>

#include <Utils/StringUtils.h>

void PlayerSettingPanel::OnImGuiRender(WorldEditor* worldEditor)
{
    DrawGameplayDetails(worldEditor);
}

void PlayerSettingPanel::DrawGameplayDetails(WorldEditor* worldEditor)
{
    DrawMainCameraDetails(worldEditor);

    DrawKeyMappingEditor(*worldEditor->m_scene->m_keyMapping.get());
}

void PlayerSettingPanel::DrawMainCameraDetails(WorldEditor* worldEditor)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header1);
    
    if (ImGui::TreeNodeEx("Main Camera", flags))
    {
        EditorUI::FontStyles::Pop();

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Main camera");
        EditorUI::FontStyles::Pop();

        SE::UUID currentUUID = worldEditor->m_scene->m_mainCameraUUID;
        worldEditor->m_miniViewRenderer->SetMainCamera(
            worldEditor->m_scene->m_cameraManager->GetCameraByUUID(currentUUID));

        int currentIndex = 0;
        int i = 0;
        for (auto cam : worldEditor->m_scene->m_cameraManager->m_camerasUUID) {
            if (currentUUID == cam.m_UUID) {
                currentIndex = i;
            }
        }

        ImGui::Text("Main camera:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-FLT_MIN);

        auto size = worldEditor->m_scene->m_cameraManager->m_camerasUUID.size();

        std::string preview = size > 0 ?
            worldEditor->m_scene->m_mainCameraUUID.ToString() : "None";

        if (ImGui::BeginCombo("##ParentCombo", preview.c_str())) {
            for (int i = 0; i < (int)size; ++i) {
                bool selected = (i == currentIndex);
                if (ImGui::Selectable(worldEditor->m_scene->m_cameraManager->m_camerasUUID[i].ToString().c_str(),
                    selected)) {

                    currentIndex = i;
                    worldEditor->m_scene->m_mainCameraUUID =
                        worldEditor->m_scene->m_cameraManager->m_camerasUUID[i];
                    worldEditor->m_miniViewRenderer->SetMainCamera(
                        worldEditor->m_scene->m_cameraManager->GetCameraByUUID(
                            worldEditor->m_scene->m_mainCameraUUID));
                }
                if (selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("View");
        EditorUI::FontStyles::Pop();

        ImVec2 avail = ImGui::GetContentRegionAvail();
        avail.y = avail.x * 360.0f / 640.0f;

        ImGui::Image((ImTextureID) worldEditor->m_miniViewRenderer->m_GBuffer->pLightSRV.Get(), avail);

        /*
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

void PlayerSettingPanel::DrawKeyMappingEditor(PlayerInputSystem::KeyMapping_Info& mapping)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header1);

    // KeyMapping settings
    if (ImGui::TreeNodeEx("Key mapping settings", flags))
    {
        EditorUI::FontStyles::Pop();

        ImGui::PushID(&mapping);

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        if (ImGui::CollapsingHeader(
            "Action Bindings",
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            EditorUI::FontStyles::Pop();
            DrawActionBindings(mapping);
        }
        else
        {
            EditorUI::FontStyles::Pop();
        }

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        if (ImGui::CollapsingHeader(
            "Axis Bindings",
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            EditorUI::FontStyles::Pop();
            DrawAxisBindings(mapping);
        }
        else
        {
            EditorUI::FontStyles::Pop();
        }

        ImGui::PopID();
        ImGui::TreePop();
    }
    else
    {
        ImGui::TreePop();
        EditorUI::FontStyles::Pop();
    }
}

void PlayerSettingPanel::DrawActionBindings(PlayerInputSystem::KeyMapping_Info& mapping)
{
    auto& bindings =
        mapping.GetActionBindings();

    if (ImGui::Button("+ Add Action"))
    {
        bindings.push_back(
            {
                Keys::None,
                "NewAction"
            });
    }

    ImGui::Separator();

    if (!ImGui::BeginTable(
        "ActionBindings",
        3,
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_Resizable))
    {
        return;
    }

    ImGui::TableSetupColumn("Key",
        ImGuiTableColumnFlags_WidthFixed,
        120.0f);
    ImGui::TableSetupColumn(
        "Action",
        ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn(
        "",
        ImGuiTableColumnFlags_WidthFixed,
        70.0f);

    ImGui::TableHeadersRow();

    int removeIndex = -1;

    struct PendingRename
    {
        size_t idx;
        Keys oldKey;
        Keys newKey;
    };
    std::vector<PendingRename> renames;

    auto cache =
        BuildActionConflictCache(bindings);

    for (size_t i = 0; i < bindings.size(); ++i)
    {
        auto& binding =
            bindings[i];

        //ImGui::PushID((KeyToName(binding.Key) + binding.Action).c_str());
        ImGui::PushID(i);

        ImGui::TableNextRow();

        //
        // Key
        //

        ImGui::TableSetColumnIndex(0);

        PendingRename ren = { i, binding.Key, binding.Key };
        ImGui::SetNextItemWidth(-FLT_MIN);
        DrawKeyCombo(
            "##Key", ren.oldKey, ren.newKey);

        if (ren.oldKey != ren.newKey) {
            renames.push_back(ren);
        }

        bool conflict =
            HasActionConflict(
                cache,
                binding.Key);
        if (conflict)
        {
            ImGui::TableSetBgColor(
                ImGuiTableBgTarget_RowBg0,
                IM_COL32(255, 100, 100, 80));

            /*
            ImGui::TextColored(
                ImVec4(1.f, 0.8f, 0.f, 1.f),
                "Conflict");
            */

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Key assigned with %d actions", cache.KeyUsage[binding.Key]);
                ImGui::EndTooltip();
            }
        }

        //
        // Action
        //

        ImGui::TableSetColumnIndex(1);

        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText(
            "##Action",
            &binding.Action);

        //
        // Delete
        //

        ImGui::TableSetColumnIndex(2);

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::SmallButton("Delete"))
        {
            removeIndex =
                static_cast<int>(i);
        }

        ImGui::PopID();
    }

    ImGui::EndTable();

    for (auto ren : renames) {
        bindings[ren.idx].Key = ren.newKey;
    }

    if (removeIndex >= 0)
    {
        bindings.erase(
            bindings.begin() +
            removeIndex);
    }
}

void PlayerSettingPanel::DrawAxisBindings(PlayerInputSystem::KeyMapping_Info& mapping)
{
    auto& bindings =
        mapping.GetAxisBindings();

    if (ImGui::Button("+ Add Axis"))
    {
        bindings.push_back(
            {
                Keys::None,
                "NewAxis",
                1.0f
            });
    }

    ImGui::Separator();

    if (!ImGui::BeginTable(
        "AxisBindings",
        4,
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_Resizable))
    {
        return;
    }

    ImGui::TableSetupColumn(
        "Key",
        ImGuiTableColumnFlags_WidthFixed,
        120.0f);
    ImGui::TableSetupColumn(
        "Axis",
        ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn(
        "Scale",
        ImGuiTableColumnFlags_WidthFixed,
        100.0f);
    ImGui::TableSetupColumn(
        "",
        ImGuiTableColumnFlags_WidthFixed,
        70.0f);

    ImGui::TableHeadersRow();

    int removeIndex = -1;

    struct PendingRename
    {
        size_t idx;
        Keys oldKey;
        Keys newKey;
    };
    std::vector<PendingRename> renames;

    auto cache = BuildAxisConflictCache(bindings);

    for (size_t i = 0; i < bindings.size(); ++i)
    {
        auto& binding =
            bindings[i];

        //ImGui::PushID((KeyToName(binding.Key) + binding.Name).c_str());
        ImGui::PushID(i);

        ImGui::TableNextRow();

        //
        // Key
        //

        ImGui::TableSetColumnIndex(0);

        PendingRename ren = { i, binding.Key, binding.Key };
        ImGui::SetNextItemWidth(-FLT_MIN);
        DrawKeyCombo(
            "##Key", ren.oldKey, ren.newKey);

        bool conflict =
            HasAxisConflict(
                cache,
                binding);
        if (conflict)
        {
            ImGui::TableSetBgColor(
                ImGuiTableBgTarget_RowBg0,
                IM_COL32(255, 100, 100, 80));

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Key assigned with %d actions",
                    cache.Usage[AxisConflictKey{ binding.Key, binding.Name }]
                );
                ImGui::EndTooltip();
            }
        }


        if (ren.oldKey != ren.newKey) {
            renames.push_back(ren);
        }

        //
        // Axis name
        //

        ImGui::TableSetColumnIndex(1);

        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText(
            "##Axis",
            &binding.Name);

        //
        // Scale
        //

        ImGui::TableSetColumnIndex(2);

        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragFloat(
            "##Scale",
            &binding.Scale,
            0.1f,
            -10.0f,
            10.0f);

        //
        // Delete
        //

        ImGui::TableSetColumnIndex(3);

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::SmallButton("Delete"))
        {
            removeIndex =
                static_cast<int>(i);
        }

        ImGui::PopID();
    }

    ImGui::EndTable();

    for (auto ren : renames) {
        bindings[ren.idx].Key = ren.newKey;
    }

    if (removeIndex >= 0)
    {
        bindings.erase(
            bindings.begin() +
            removeIndex);
    }
}

bool PlayerSettingPanel::DrawKeyCombo(
    const char* label,
    Keys& key, Keys& newKey)
{
    bool changed = false;

    if (ImGui::BeginCombo(label, KeyToName(key).c_str()))
    {
        for (const KeyInfo& info : g_AllKeys)
        {
            bool selected = (info.Key == key);

            if (ImGui::Selectable(info.Name, selected))
            {
                newKey = info.Key;
                changed = true;
            }

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return changed;
}

PlayerSettingPanel::ActionConflictCache PlayerSettingPanel::BuildActionConflictCache(const std::vector<PlayerInputSystem::ActionBinding>& bindings)
{
    ActionConflictCache cache;

    cache.KeyUsage.reserve(bindings.size());

    for (const auto& binding : bindings)
    {
        if (binding.Key == Keys::None)
            continue;

        ++cache.KeyUsage[binding.Key];
    }

    return cache;
}

bool PlayerSettingPanel::HasActionConflict(
    const ActionConflictCache& cache,
    Keys key)
{
    if (key == Keys::None)
        return false;

    auto it = cache.KeyUsage.find(key);

    return it != cache.KeyUsage.end() &&
        it->second > 1;
}

PlayerSettingPanel::AxisConflictCache PlayerSettingPanel::BuildAxisConflictCache(const std::vector<PlayerInputSystem::AxisBinding>& bindings)
{
    AxisConflictCache cache;

    cache.Usage.reserve(bindings.size());

    for (const auto& binding : bindings)
    {
        if (binding.Key == Keys::None)
            continue;

        AxisConflictKey key
        {
            binding.Key,
            binding.Name
        };

        ++cache.Usage[key];
    }

    return cache;
}

bool PlayerSettingPanel::HasAxisConflict(
    const AxisConflictCache& cache,
    const PlayerInputSystem::AxisBinding& binding)
{
    if (binding.Key == Keys::None)
        return false;

    AxisConflictKey key
    {
        binding.Key,
        binding.Name
    };

    auto it = cache.Usage.find(key);

    return it != cache.Usage.end() &&
        it->second > 1;
}
