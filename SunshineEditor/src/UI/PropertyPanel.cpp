#include "UI/PropertyPanel.h"
#include "WorldEditor.h"
#include "GameObject.h"
#include "Component/TransformComponent.h"
#include "Component/LuaComponent.h"
#include "DirectXMath.h"
<<<<<<< Updated upstream
#include "Graphics/Renderer/Technique/LightTechnique.h"
#include "Graphics/Lighting/LightCollection.h"
=======
#include "GameObject/Lighting/LightObject.h"
>>>>>>> Stashed changes
#include "Graphics/Lighting/LightData.h"
#include "Graphics/Renderer/Technique/PointLightTechnique.h"

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
    DrawDetails(obj);
    DrawLuaComponent(obj);
    
    DrawComponentAddPopup(obj);

    ImGui::End();
}

void PropertyPanel::DrawGameObjectHeader(GameObject* obj)
{
    ImGui::Text("GameObject");
    ImGui::SameLine();
    
    static char nameBuffer[256] = {0};
    strncpy(nameBuffer, obj->Name.c_str(), sizeof(nameBuffer) - 1);
    
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

<<<<<<< Updated upstream
void PropertyPanel::DrawRenderComponent(GameObject* obj)
{
    if (!obj->HasComponent<RenderComponent>()) 
        return;

    auto render = obj->GetComponent<RenderComponent>();
    
=======
void PropertyPanel::DrawDetails(GameObject_Info* obj)
{
>>>>>>> Stashed changes
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | 
                              ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth;
    
    if (ImGui::TreeNodeEx("Details", flags))
    {  
        if (obj->m_group == GameObjectGroup::Lighting)
        {
<<<<<<< Updated upstream
            for (auto& techPair : render->techniques)
            {
                ImGui::PushID(techPair.first.c_str());
                
                bool isSelected = (selectedTechnique == techPair.first);
                
                const char* type = "";
                
                if (auto gpass = dynamic_cast<SE_G::GPassTechnique*>(techPair.second.get())) 
                    type = "Geometry";
                else if (auto iconTech = dynamic_cast<SE_G::IconTechnique*>(techPair.second.get())) 
                    type = "Icon";
                if (auto ambient = dynamic_cast<SE_G::LightTechnique<SE_G::AmbientLightData>*>(techPair.second.get())) 
                    type = "Ambient Light";
                else if (auto directional = dynamic_cast<SE_G::LightTechnique<SE_G::DirectionalLightData>*>(techPair.second.get())) 
                    type = "Directional Light";
                else if (auto point = dynamic_cast<SE_G::LightTechnique<SE_G::PointLightData>*>(techPair.second.get())) 
                    type = "Point Light";
                else if (auto skybox = dynamic_cast<SE_G::LightTechnique<SE_G::SkyBoxData>*>(techPair.second.get())) 
                    type = "Skybox";
                
                if (isSelected)
=======
            switch (obj->m_type.m_asLight)
            {
            case LightObjectType::AmbientLight:
                if (auto lightObj = dynamic_cast<LightObject_Info<SE_G::AmbientLightData>*>(obj))
>>>>>>> Stashed changes
                {
                    DrawAmbientLightDetails(lightObj->m_lightData.get());
                }
                break;
                
            case LightObjectType::DirectionalLight:
                if (auto lightObj = dynamic_cast<LightObject_Info<SE_G::DirectionalLightData>*>(obj))
                {
<<<<<<< Updated upstream
                    selectedTechnique = techPair.first;
=======
                    DrawDirectionalLightDetails(lightObj->m_lightData.get());
>>>>>>> Stashed changes
                }
                break;
                
            case LightObjectType::PointLight:
                if (auto lightObj = dynamic_cast<LightObject_Info<SE_G::PointLightData>*>(obj))
                {
                    DrawPointLightDetails(lightObj->m_lightData.get());
                }
                break;
                
            case LightObjectType::SkyBox:
                if (auto lightObj = dynamic_cast<LightObject_Info<SE_G::SkyBoxData>*>(obj))
                {
                    DrawSkyBoxDetails(lightObj->m_lightData.get());
                }
                break;
                
            default:
                break;
            }
        }
<<<<<<< Updated upstream
        ImGui::EndChild();
        
        if (!selectedTechnique.empty() && render->techniques.find(selectedTechnique) != render->techniques.end())
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::Text("Selected type:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "%s", selectedTechnique.c_str());
            
            auto& tech = render->techniques[selectedTechnique];
            DrawTechniqueDetails(tech.get(), selectedTechnique);
        }
        else if (!render->techniques.empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("Select a type to view details");
        }
=======
>>>>>>> Stashed changes
        
        ImGui::TreePop();
    }
}

void PropertyPanel::DrawAmbientLightDetails(SE_G::AmbientLightData* lightData)
{
    if (lightData)
    {
        ImGui::Text("Ambient Light");
        
        ImGui::ColorEdit3("Light Color", &lightData->Ambient.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Intensity", &lightData->AmbientPad, 0.01f, 0.0f, 10.0f, "%.2f");
    }
}

void PropertyPanel::DrawDirectionalLightDetails(SE_G::DirectionalLightData* lightData)
{
    if (lightData)
    {
        ImGui::Text("Directional Light");
        
        ImGui::ColorEdit3("Light Color", &lightData->Diffuse.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Intensity", &lightData->DiffusePad, 0.1f, 0.0f, 50.0f, "%.1f");
    }
}

void PropertyPanel::DrawPointLightDetails(SE_G::PointLightData* lightData)
{
    if (lightData)
    {
        ImGui::Text("Point Light");
        
        ImGui::ColorEdit3("Light Color", &lightData->Diffuse.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Intensity", &lightData->DiffusePad, 0.1f, 0.0f, 100.0f, "%.1f");
        ImGui::DragFloat("Range", &lightData->Range, 0.5f, 0.0f, 100.0f, "%.1f m");
    }
}

void PropertyPanel::DrawSkyBoxDetails(SE_G::SkyBoxData* lightData)
{
    if (lightData)
    {
        ImGui::Text("Skybox - Environment Background");
        
<<<<<<< Updated upstream
        ImGui::ColorEdit3("Sky Tint", &data.Tint.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Brightness", &data.Power, 0.1f, 0.0f, 10.0f, "%.1f");
        
        if (tech->texture)
        {
            ImGui::Text("Skybox Texture: Loaded");
        }
        else
        {
            ImGui::TextDisabled("No skybox texture assigned");
        }
    }
}

void PropertyPanel::DrawGPassTechniqueDetails(SE_G::GPassTechnique* tech)
{
    ImGui::Text("Geometry - Object Rendering");
}

void PropertyPanel::DrawIconTechniqueDetails(SE_G::IconTechnique* tech)
{
    ImGui::Text("Editor Icon");
}

void PropertyPanel::DrawLuaComponent(GameObject* obj)
=======
        ImGui::ColorEdit3("Sky Tint", &lightData->Tint.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Brightness", &lightData->Power, 0.1f, 0.0f, 10.0f, "%.1f");
    }
}

void PropertyPanel::DrawLuaComponent(GameObject_Info* obj)
>>>>>>> Stashed changes
{
    if (!obj->HasComponent<LuaComponent>()) 
        return;

    auto luaComp = obj->GetComponent<LuaComponent>();
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | 
                              ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth;
    
    if (ImGui::TreeNodeEx("Lua Script", flags))
    {
        ImGui::Text("Script Selection");
        ImGui::Separator();
        
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
        
        ImGui::Spacing();
        ImGui::Text("Script Control");
        ImGui::Separator();
        
        eastl::string currentScriptPath = luaComp->assetsPath + "/" + luaComp->luaFiles[luaComp->selectedLuaFile];
        bool isCurrentFileLoaded = luaComp->scriptLoaded && (luaComp->scriptPath == currentScriptPath);
        
        ImGui::BeginGroup();
        ImGui::Text("Status:");
        ImGui::SameLine();
        ImGui::TextColored(isCurrentFileLoaded ? 
          ImVec4(0, 1, 0, 1) : ImVec4(1, 0.5f, 0, 1), 
          isCurrentFileLoaded ? "Loaded" : "Not Loaded");
        ImGui::EndGroup();

        ImGui::BeginGroup();
        if (isCurrentFileLoaded)
        {
            if (ImGui::Button("Reload", ImVec2(120, 0)))
            {
                luaComp->Cleanup();
                luaComp->LoadScript();
            }
        }
        else
        {
            if (ImGui::Button("Load Script", ImVec2(120, 0))) 
            {
                luaComp->LoadScript();
            }
        }
        
        ImGui::EndGroup();

        if (isCurrentFileLoaded)
        {
            ImGui::Spacing();
            DrawLuaFunctions(luaComp.get());
        }
        
        ImGui::TreePop();
    }
}

void PropertyPanel::DrawLuaFunctions(LuaComponent* luaComp)
{
    ImGui::Text("Script Functions");
    ImGui::Separator();

    auto availableFunctions = luaComp->GetAvailableFunctions();

    if (availableFunctions.empty())
    {
        ImGui::TextDisabled("No functions found in script");
        return;
    }
    
    ImGui::Text("Available Functions:");
    
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

    ImGui::Spacing();
    if (luaComp->foundFunction)
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Function loaded");
        ImGui::SameLine();
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No function selected");
        ImGui::SameLine();
        ImGui::TextDisabled("(Choose from list above)");
    }
    
    if (luaComp->foundFunction)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Function Parameters:");
        
        for (int i = 0; i < luaComp->params.size(); ++i)
        {
            auto& param = luaComp->params[i];

            ImGui::PushID(i);
            
            ImGui::Text("%s:", param.name.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("(%s)", param.type.c_str());
            
            if (EASTLStringEqualsChar(param.type, "userdata"))
            {
                ImGui::TextDisabled("GameObject");
            }
            else
            {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
                
                ImGui::InputText(("##param_" + eastl::to_string(i)).c_str(), 
                               param.value, sizeof(param.value));
            }

            ImGui::PopID();
        }
        
        ImGui::Spacing();
        if (ImGui::Button("Call Function", ImVec2(120, 30)))
        {
            luaComp->CallFunction();
        }
        
        if (!luaComp->lastResult.empty())
        {
            ImGui::BeginGroup();
            ImGui::Text("Result:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0.5f, 1), "%s", luaComp->lastResult.c_str());
            ImGui::EndGroup();
        }
    }
}

void PropertyPanel::DrawComponentAddPopup(GameObject* obj)
{
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    
    if (ImGui::Button("Add Component", ImVec2(-1, 30)))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }
    
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        ImGui::Text("Add Component");
        ImGui::Separator();
        
        if (!obj->HasComponent<LuaComponent>())
        {
            if (ImGui::MenuItem("Lua Script", nullptr, false, true))
            {
                obj->AddComponent<LuaComponent>();
                auto luaComp = obj->GetComponent<LuaComponent>();
                luaComp->Init(obj);
            }
        }
<<<<<<< Updated upstream
        else if (!obj->HasComponent<RenderComponent>())
        {
            if (ImGui::MenuItem("Render Component", nullptr, false, true))
            {
                obj->AddComponent<RenderComponent>();
            }
        }
=======
>>>>>>> Stashed changes
        else
        {
            ImGui::TextDisabled("All available components added");
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