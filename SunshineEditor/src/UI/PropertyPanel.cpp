#include "UI/PropertyPanel.h"
#include "WorldEditor.h"
#include "GameObject/GameObject.h"
#include "Component/TransformComponent.h"
#include "Component/LuaComponent.h"
#include "DirectXMath.h"
#include "Graphics/Renderer/Technique/LightTechnique.h"
#include "GameObject/Lighting/LightCollection.h"
#include "GameObject/Lighting/LightObject.h"
#include "Graphics/Lighting/LightData.h"
#include "Graphics/Renderer/Technique/PointLightTechnique.h"

PropertyPanel::PropertyPanel() {}

void PropertyPanel::OnImGuiRender()
{
    ImGui::Begin("Properties");
    
    if (m_SelectedUUID == SE::UUID(0u) || !m_WorldEditor)
    {
        ImGui::Text("No object selected");
        ImGui::End();
        return;
    }

    GameObject_Info* obj = m_WorldEditor->m_scene->GetGameObjectByUUID(m_SelectedUUID);
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

void PropertyPanel::DrawGameObjectHeader(GameObject_Info* obj)
{
    ImGui::Text("GameObject");
    ImGui::SameLine();
    
    static char nameBuffer[256] = {0};
    strncpy(nameBuffer, obj->m_name.c_str(), sizeof(nameBuffer) - 1);
    
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
    {
        obj->m_name = nameBuffer;
    }
    
    ImGui::TextDisabled("UUID: %llu", obj->m_UUID.m_UUID);
}

void PropertyPanel::DrawTransformComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<TransformComponent_Info>()) 
        return;

    auto transform = obj->GetComponent<TransformComponent_Info>()->m_assignedComponent.get();
    
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

void PropertyPanel::DrawDetails(GameObject_Info* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx("Details", flags))
    {
        if (obj->m_group == GameObjectGroup::Lighting)
        {
            switch (obj->m_type.m_asLight)
            {
            case LightObjectType::AmbientLight:
                if (auto lightObj = dynamic_cast<LightObject_Info<SE_G::AmbientLightData>*>(obj))
                {
                    DrawAmbientLightDetails(lightObj->m_lightData.get());
                }
                break;

            case LightObjectType::DirectionalLight:
                if (auto lightObj = dynamic_cast<LightObject_Info<SE_G::DirectionalLightData>*>(obj))
                {
                    DrawDirectionalLightDetails(lightObj->m_lightData.get());
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
        else if (obj->m_group == GameObjectGroup::Shapes)
        {
            switch (obj->m_type.m_asShape)
            {
            case ShapeObjectType::Box:
                if (auto shapeObj = dynamic_cast<BoxShapeObject_Info*>(obj))
                {
                    DrawBoxShapeDetails(shapeObj);
                }
                break;

            case ShapeObjectType::Sphere:
                if (auto shapeObj = dynamic_cast<SphereShapeObject_Info*>(obj))
                {
                    DrawSphereShapeDetails(shapeObj);
                }
                break;
                
            default:
                break;
            }
        }
        ImGui::TreePop();
    }
}

void PropertyPanel::DrawAmbientLightDetails(SE_G::AmbientLightData* lightData)
{
    if (lightData)
    {
        ImGui::Text("Ambient Light");
        ImGui::ColorEdit3("Light Color", &lightData->Ambient.x, ImGuiColorEditFlags_Float);
    }
}

void PropertyPanel::DrawDirectionalLightDetails(SE_G::DirectionalLightData* lightData)
{
    if (lightData)
    {
        ImGui::Text("Directional Light");
        
        ImGui::ColorEdit3("Diffuse Color", &lightData->Diffuse.x, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Specular Color", &lightData->Specular.x, ImGuiColorEditFlags_Float);
        // direction should be normalized after edditing! Use angles instead of coordinates? Hmmm...
        // ImGui::ColorEdit3("Light Color", &lightData->Direction.x, ImGuiColorEditFlags_Float);
    }
}

void PropertyPanel::DrawPointLightDetails(SE_G::PointLightData* lightData)
{
    if (lightData)
    {
        ImGui::Text("Point Light");
        
        ImGui::ColorEdit3("Diffuse Color", &lightData->Diffuse.x, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Specular Color", &lightData->Specular.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Range", &lightData->Range, 0.5f, 0.0f, 100.0f, "%.1f m");
    }
}

void PropertyPanel::DrawSkyBoxDetails(SE_G::SkyBoxData* lightData)
{
    if (lightData)
    {
        ImGui::Text("Skybox");
        
        ImGui::ColorEdit3("Sky Tint", &lightData->Tint.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Intensity", &lightData->Power, 0.1f, 0.0f, 10.0f, "%.1f");
    }
}

void PropertyPanel::DrawBoxShapeDetails(BoxShapeObject_Info* obj)
{
    ImGui::Text("Box Shape");
    
    DXSM::Vector3 currentSize = obj->GetSize();
    
    if (DrawVector3Control("Size", currentSize, 1.0f))
    {
        obj->SetSize(m_WorldEditor->m_renderer.get(), currentSize);
    }
}

void PropertyPanel::DrawSphereShapeDetails(SphereShapeObject_Info* obj)
{
    ImGui::Text("Sphere Shape");
    
    DXSM::Vector3 currentSize = obj->GetSize();
    uint32_t currentSliceCount = obj->GetSliceCount();
    uint32_t currentStackCount = obj->GetStackCount();
    
     if (DrawVector3Control("Size", currentSize, 1.0f))
     {
         obj->SetSize(m_WorldEditor->m_renderer.get(), currentSize);
     }

    uint32_t min_slice = 3, max_slice = 64;
    if (DrawUIntControl("Slice Count", currentSliceCount, 10, 1.0f, min_slice, max_slice))
    {
        obj->SetSliceCount(m_WorldEditor->m_renderer.get(), currentSliceCount);
    }
    
    uint32_t min_stack = 3, max_stack = 64;
    if (DrawUIntControl("Stack Count", currentStackCount, 10, 1.0f, min_stack, max_stack))
    {
        obj->SetStackCount(m_WorldEditor->m_renderer.get(), currentStackCount);
    }
}

void PropertyPanel::DrawLuaComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<LuaComponent_Info>()) 
        return;

    auto luaComp = obj->GetComponent<LuaComponent_Info>();
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | 
                              ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth;
    /*
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
    */
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

void PropertyPanel::DrawComponentAddPopup(GameObject_Info* obj)
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
        /*
        if (!obj->HasComponent<LuaComponent_Info>())
        {
            if (ImGui::MenuItem("Lua Script", nullptr, false, true))
            {
                obj->AddComponent<LuaComponent_Info>();
                auto luaComp = obj->GetComponent<LuaComponent_Info>();
                luaComp->Init(obj);
            }
        }
        else
        {
            ImGui::TextDisabled("All available components added");
        }
        */
        ImGui::EndPopup();
    }
}

bool PropertyPanel::DrawFloatControl(const char* label, float& value, float resetValue,
                                   float speed, float min, float max, 
                                   const char* format, float columnWidth)
{
    bool changed = false;
    
    ImGui::PushID(label);
    
    if (ImGui::BeginTable(label, 2, 
                         ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV |
                         ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoSavedSettings))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
        ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);
        
        ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetFrameHeight());
        ImGui::TableNextColumn();
        
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        
        ImGui::TableNextColumn();
        
        ImGui::AlignTextToFramePadding();
        
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float inputWidth = availableWidth - 30.0f;
        
        ImGui::SetNextItemWidth(inputWidth);
        if (ImGui::DragFloat("##Value", &value, speed, min, max, format))
            changed = true;
        
        ImGui::SameLine(0, 4);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.4f, 0.4f, 0.6f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.5f, 0.5f, 0.5f, 0.8f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.3f, 0.3f, 0.3f, 0.6f});
        
        if (ImGui::Button("R", ImVec2(25, ImGui::GetFrameHeight())))
        {
            value = resetValue;
            changed = true;
        }
        
        ImGui::PopStyleColor(3);
        
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Reset to default");
        
        ImGui::EndTable();
    }
    
    ImGui::PopID();
    
    return changed;
}

bool PropertyPanel::DrawUIntControl(const char* label, uint32_t& value, uint32_t resetValue,
                                  float speed, uint32_t min, uint32_t max,
                                  const char* format, float columnWidth)
{
    bool changed = false;
    
    ImGui::PushID(label);
    
    if (ImGui::BeginTable(label, 2, 
                         ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV |
                         ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoSavedSettings))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
        ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);
        
        ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetFrameHeight());
        ImGui::TableNextColumn();
        
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        
        ImGui::TableNextColumn();
        
        ImGui::AlignTextToFramePadding();
        
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float inputWidth = availableWidth - 30.0f;
        
        ImGui::SetNextItemWidth(inputWidth);
        if (ImGui::DragScalar("##Value", ImGuiDataType_U32, &value, speed, &min, &max, format))
            changed = true;
        
        ImGui::SameLine(0, 4);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.4f, 0.4f, 0.6f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.5f, 0.5f, 0.5f, 0.8f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.3f, 0.3f, 0.3f, 0.6f});
        
        if (ImGui::Button("R", ImVec2(25, ImGui::GetFrameHeight())))
        {
            value = resetValue;
            changed = true;
        }
        
        ImGui::PopStyleColor(3);
        
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Reset to default");
        
        ImGui::EndTable();
    }
    
    ImGui::PopID();
    
    return changed;
}

bool PropertyPanel::DrawVector3Control(const char* label, DirectX::SimpleMath::Vector3& values, 
                                      float resetValue, float columnWidth)
{
    bool changed = false;
    
    ImGui::PushID(label);
    
    if (ImGui::BeginTable(label, 2, 
                         ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV |
                         ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoSavedSettings))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
        ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);
        
        ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetFrameHeight());
        ImGui::TableNextColumn();
        
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        
        ImGui::TableNextColumn();
        
        ImGui::AlignTextToFramePadding();
        
        float totalWidth = ImGui::GetContentRegionAvail().x;
        float itemWidth = (totalWidth - 60.0f) / 3.0f;
        
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{2, 0});
        
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = { 20.0f, lineHeight };
        
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
        
        ImGui::SameLine(0, 4);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.4f, 0.4f, 0.6f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.5f, 0.5f, 0.5f, 0.8f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.3f, 0.3f, 0.3f, 0.6f});
        
        if (ImGui::Button("R", ImVec2(25, ImGui::GetFrameHeight())))
        {
            values = DirectX::SimpleMath::Vector3(resetValue, resetValue, resetValue);
            changed = true;
        }
        
        ImGui::PopStyleColor(3);
        
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Reset to default");
        
        ImGui::PopStyleVar();
        
        ImGui::EndTable();
    }
    
    ImGui::PopID();
    
    return changed;
}