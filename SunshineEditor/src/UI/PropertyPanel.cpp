#include "UI/PropertyPanel.h"
#include "WorldEditor.h"
#include "GameObject.h"
#include "Component/TransformComponent.h"
#include "Component/RenderComponent.h"
#include "Component/LuaComponent.h"
#include <Utils/DebugUtils.h>
#include "DirectXMath.h"

PropertyPanel::PropertyPanel() {}

void PropertyPanel::OnImGuiRender()
{
    ImGui::Begin("Properties");
    
    if (m_SelectedUUID == Sunshine::UUID(0u) || !m_WorldEditor)
    {
        ImGui::Text("No object selected");
        ImGui::End();
        return;
    }

    GameObject* obj = m_WorldEditor->m_scene.GetGameObjectByUUID(m_SelectedUUID);
    if (!obj)
    {
        ImGui::Text("Object not found");
        ImGui::End();
        return;
    }
    
    DrawGameObjectHeader(obj);
    ImGui::Separator();
    
    DrawTransformComponent(obj);
    DrawRenderComponent(obj);
    DrawLuaComponent(obj);
    
    DrawComponentAddPopup(obj);

    ImGui::End();
}

void PropertyPanel::DrawGameObjectHeader(GameObject* obj)
{
    ImGui::Text("GameObject");
    ImGui::SameLine();
    
    static char nameBuffer[256] = {0};
    if (ImGui::IsWindowAppearing())
    {
        strncpy(nameBuffer, obj->Name.c_str(), sizeof(nameBuffer) - 1);
    }
    
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
    {
        obj->Name = nameBuffer;
    }
    
    ImGui::TextDisabled("UUID: %llu", obj->m_UUID.m_UUID);
}

void PropertyPanel::DrawTransformComponent(GameObject* obj)
{
    if (!obj->HasComponent<TransformComponent>()) 
        return;

    auto transform = obj->GetComponent<TransformComponent>();
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | 
                              ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth;
    
    if (ImGui::TreeNodeEx("Transform", flags))
    {
        DrawVector3Control("Position", transform->m_position, 0.0f);
        
        DXSM::Vector3 rotationDeg = transform->m_rotation * (180.0f / DirectX::XM_PI);
        if (DrawVector3Control("Rotation", rotationDeg, 0.0f))
        {
            transform->m_rotation = rotationDeg * (DirectX::XM_PI / 180.0f);
        }
        
        DrawVector3Control("Scale", transform->m_scaleFactor, 1.0f);
        
        if (ImGui::TreeNode("Local Transform"))
        {
            DrawVector3Control("Local Position", transform->m_localPosition, 0.0f);
            
            DXSM::Vector3 localRotDeg = transform->m_localRotation * (180.0f / DirectX::XM_PI);
            if (DrawVector3Control("Local Rotation", localRotDeg, 0.0f))
            {
                transform->m_localRotation = localRotDeg * (DirectX::XM_PI / 180.0f);
            }
            
            DrawVector3Control("Local Scale", transform->m_localScaleFactor, 1.0f);
            
            ImGui::TreePop();
        }
        
        ImGui::TreePop();
    }
}

void PropertyPanel::DrawRenderComponent(GameObject* obj)
{
    if (!obj->HasComponent<RenderComponent>()) 
        return;

    auto render = obj->GetComponent<RenderComponent>();
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | 
                              ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth;
    
    if (ImGui::TreeNodeEx("Render", flags))
    {
        ImGui::Text("Techniques: %zu", render->techniques.size());
        
        for (auto& techPair : render->techniques)
        {
            if (ImGui::TreeNode(techPair.first.c_str()))
            {
                auto& tech = techPair.second;
                
                if (tech->vertexShader)
                    ImGui::Text("Vertex Shader: Loaded");
                if (tech->pixelShader)
                    ImGui::Text("Pixel Shader: Loaded");
                if (tech->mesh)
                    ImGui::Text("Mesh: Present");
                
                ImGui::TreePop();
            }
        }
        
        ImGui::TreePop();
    }
}

void PropertyPanel::DrawLuaComponent(GameObject* obj)
{
    if (!obj->HasComponent<LuaComponent>()) 
        return;

    auto luaComp = obj->GetComponent<LuaComponent>();
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | 
                              ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth;
    
    if (ImGui::TreeNodeEx("Lua Script", flags))
    {
        if (!luaComp->luaFiles.empty())
        {
            ImGui::Text("Script File:");
            ImGui::SameLine();
    
            float comboWidth = ImGui::GetContentRegionAvail().x * 0.6f; 
            ImGui::SetNextItemWidth(comboWidth);
            
            if (ImGui::BeginCombo("##Script File", 
                luaComp->luaFiles[luaComp->selectedLuaFile].c_str()))
            {
                for (int i = 0; i < luaComp->luaFiles.size(); ++i)
                {
                    bool isSelected = (i == luaComp->selectedLuaFile);
                    if (ImGui::Selectable(luaComp->luaFiles[i].c_str(), isSelected))
                    {
                        luaComp->selectedLuaFile = i;
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        else
        {
            ImGui::TextDisabled("No Lua files found");
        }
        
        ImGui::BeginGroup();
        
        eastl::string currentScriptPath = luaComp->assetsPath + "/" + luaComp->luaFiles[luaComp->selectedLuaFile];
        bool isCurrentFileLoaded = luaComp->scriptLoaded && (luaComp->scriptPath == currentScriptPath);
        
        if (isCurrentFileLoaded)
        {
            ImGui::BeginDisabled();
            ImGui::Button("Load Script");
            ImGui::EndDisabled();
            
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Script is already loaded");
            }

            ImGui::SameLine();
            if (ImGui::Button("Reload"))
            {
                luaComp->Cleanup();
                luaComp->LoadScript();
            }
        }
        else
        {
            if (ImGui::Button("Load Script")) 
            {
                luaComp->LoadScript();
            }
        }
        
        
        ImGui::EndGroup();
        
        ImGui::SameLine();
        ImGui::TextColored(isCurrentFileLoaded ? 
            ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), 
            isCurrentFileLoaded ? "Loaded" : "Not Loaded");
        
        if (isCurrentFileLoaded)
        {
            DrawLuaFunctions(luaComp.get());
        }
        
        ImGui::TreePop();
    }
}

void PropertyPanel::DrawLuaFunctions(LuaComponent* luaComp)
{
    ImGui::Separator();

    auto availableFunctions = luaComp->GetAvailableFunctions();

    if (availableFunctions.empty())
    {
        ImGui::TextDisabled("No functions found in script");
        return;
    }
    
    ImGui::Text("Select Function:");
    ImGui::SameLine();
    
    float comboWidth = ImGui::GetContentRegionAvail().x * 0.6f; 
    ImGui::SetNextItemWidth(comboWidth);
    
    static int selectedFunctionIndex = 0;
    if (ImGui::BeginCombo("##Select Function", availableFunctions[selectedFunctionIndex].c_str()))
    {
        for (int i = 0; i < availableFunctions.size(); ++i)
        {
            bool isSelected = (i == selectedFunctionIndex);
            if (ImGui::Selectable(availableFunctions[i].c_str(), isSelected))
            {
                selectedFunctionIndex = i;
                strncpy(luaComp->functionName, availableFunctions[i].c_str(), 
                       sizeof(luaComp->functionName) - 1);
                luaComp->FindFunction();
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    
    if (luaComp->foundFunction)
    {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Found");
        
        ImGui::Text("Parameters:");
        for (int i = 0; i < luaComp->params.size(); ++i)
        {
            auto& param = luaComp->params[i];
            ImGui::Text("%s (%s):", param.name.c_str(), param.type.c_str());
            ImGui::SameLine(150);
            
            if (EASTLStringEqualsChar(param.type, "userdata"))
            {
                ImGui::TextDisabled("GameObject");
            }
            else
            {
                float comboWidth = ImGui::GetContentRegionAvail().x * 0.6f; 
                ImGui::SetNextItemWidth(comboWidth);
                ImGui::InputText(("##param_" + eastl::to_string(i)).c_str(), 
                               param.value, sizeof(param.value));
            }
        }
        
        if (ImGui::Button("Call Function"))
        {
            luaComp->CallFunction();
        }
        
        if (!luaComp->lastResult.empty())
        {
            ImGui::SameLine();
            ImGui::Text("Result: %s", luaComp->lastResult.c_str());
        }
    }
}

void PropertyPanel::DrawComponentAddPopup(GameObject* obj)
{
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }
    
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        if (!obj->HasComponent<LuaComponent>())
        {
            if (ImGui::MenuItem("Lua Script"))
            {
                obj->AddComponent<LuaComponent>();
                auto luaComp = obj->GetComponent<LuaComponent>();
                luaComp->Init(obj);
            }
        }
        else if (!obj->HasComponent<RenderComponent>())
        {
            if (ImGui::MenuItem("Render Component"))
            {
                obj->AddComponent<RenderComponent>();
            }
        }
        else
        {
            ImGui::TextDisabled("No available components");
        }
        ImGui::EndPopup();
    }
}

bool PropertyPanel::DrawVector3Control(const char* label, DirectX::SimpleMath::Vector3& values, 
                                      float resetValue, float columnWidth)
{
    bool changed = false;
    
    ImGui::PushID(label);
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label);
    ImGui::NextColumn();
    
    float totalWidth = ImGui::CalcItemWidth();
    float itemWidth = totalWidth / 3.0f - ImGui::GetStyle().ItemSpacing.x * 2.0f;
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
    
    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
    
    // X
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    if (ImGui::Button("X", buttonSize))
    {
        values.x = resetValue;
        changed = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth);
    if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"))
        changed = true;
    ImGui::SameLine();
    
    // Y
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    if (ImGui::Button("Y", buttonSize))
    {
        values.y = resetValue;
        changed = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth);
    if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f"))
        changed = true;
    ImGui::SameLine();
    
    // Z
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    if (ImGui::Button("Z", buttonSize))
    {
        values.z = resetValue;
        changed = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth);
    if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f"))
        changed = true;
    
    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
    
    return changed;
}