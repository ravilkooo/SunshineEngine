#include "DirectXMath.h"

#include "WorldEditor.h"

#include <Graphics/GraphicsResources/Mesh.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Lighting/LightData.h>
#include <Graphics/Renderer/Technique/PointLightTechnique.h>
#include <Graphics/Renderer/Technique/SkyBoxTechnique.h>
#include <Graphics/Renderer/Pass/SelectionPass.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightObject.h>
#include <GameObject/Lighting/SkyBox.h>

#include <GameObject/Shapes/ShapeCollection.h>
#include <GameObject/Shapes/ShapeObject.h>

#include <PlayerObject/PlayerObject.h>

#include <ParticleSystem/ParticleEmitter.h>

#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/MeshComponent.h>
#include <Component/LuaComponent.h>

#include <SceneHierarchy.h>

#include <UI/PropertyPanel.h>
#include <UI/FontStyles.h>
#include <ResourceManager/ResourceManagerFacade.h>


PropertyPanel::MeshEditor PropertyPanel::s_meshEditor =
{
    false, "", AssetPath::AssetSource::Engine, eastl::string(""),
    false, "", AssetPath::AssetSource::Engine, eastl::string("")
};

PropertyPanel::PropertyPanel() {}

void PropertyPanel::OnImGuiRender()
{   
    if (m_SelectedUUID == SE::UUID(0u) || !m_WorldEditor)
    {
        ImGui::Text("No object selected");
        return;
    }

    GameObject_Info* obj = m_WorldEditor->m_scene->GetGameObjectByUUID(m_SelectedUUID);
    if (!obj)
    {
        ImGui::Text("Object not found");
        return;
    }
    
    if (!DrawGameObjectHeader(obj))
    {
        return;
    }
    ImGui::Separator();
    
    DrawParentnes(obj);
    ImGui::Separator();

    DrawTransformComponent(obj);
    DrawDetails(obj);
    
    DrawComponentAddPopup(obj);
}

bool PropertyPanel::DrawGameObjectHeader(GameObject_Info* obj)
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
    
    // Remove GameObject button
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 120);
    if (ImGui::Button("Remove Object", ImVec2(110, 0)))
    {
        if (m_WorldEditor && m_WorldEditor->m_scene)
        {
            SE::UUID objUUID = obj->m_UUID;
            
            // Remove from scene hierarchy
            if (m_WorldEditor->m_scene->m_sceneGraph)
            {
                m_WorldEditor->m_scene->m_sceneGraph->EraseSubtree(objUUID);
            }
            
            // Remove from scene
            m_WorldEditor->m_scene->RemoveGameObjectByUUID(objUUID);
            
            // Clear selection
            m_WorldEditor->m_hierarchySelection.picked.clear();
            m_WorldEditor->m_hierarchySelection.last_clicked = SE::UUID(0u);
            m_WorldEditor->m_selectionPass->m_selectedObjectUUID = SE::UUID(0u);
            m_SelectedUUID = SE::UUID(0u);
            return false;
        }
    }
    return true;
}

void PropertyPanel::DrawParentnes(GameObject_Info* obj)
{
    // Build list of available objects (name + uuid) for selection
    if (!m_WorldEditor || !m_WorldEditor->m_scene) {
        ImGui::Text("No scene available");
        return;
    }

    auto& scene = *m_WorldEditor->m_scene;
    eastl::vector<eastl::string> comboItems;
    comboItems.reserve(scene.gameObjects.size() + 1);

    // First entry = None
    comboItems.push_back(eastl::string("None"));

    int currentIndex = 0; // default to None
    for (int i = 0; i < (int)scene.gameObjects.size(); ++i) {
        SE::UUID u = scene.gameObjects[i];
        GameObject_Info* go = scene.GetGameObjectByUUID(u);
        char buf[512];
        if (go) snprintf(buf, sizeof(buf), "%s (%llu)", go->m_name.c_str(), u.m_UUID);
        else snprintf(buf, sizeof(buf), "Unknown (%llu)", u.m_UUID);
        comboItems.push_back(eastl::string(buf));

        if (u == obj->m_parent.uuid) currentIndex = i + 1; // +1 because of None at 0
    }

    ImGui::Text("Parent:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);
    const char* preview = comboItems.size() > 0 ? comboItems[currentIndex].c_str() : "None";
    if (ImGui::BeginCombo("##ParentCombo", preview)) {
        for (int i = 0; i < (int)comboItems.size(); ++i) {
            bool selected = (i == currentIndex);
            if (ImGui::Selectable(comboItems[i].c_str(), selected)) {
                if (i == 0) {
                    // None selected: detach
                    if (obj->m_parent.ptr) obj->DetachFromParent();
                    obj->m_parent.uuid = SE::UUID(0u);
                    obj->m_parent.ptr = nullptr;
                    obj->m_parent.attached = false;

                    m_WorldEditor->m_scene->m_sceneGraph->Reparent(obj->m_UUID, SE::UUID(0u));
                }
                else {
                    // select by UUID from scene.gameObjects[i-1]
                    SE::UUID sel = scene.gameObjects[i-1];
                    GameObject_Info* parentObj = scene.GetGameObjectByUUID(sel);
                    if (parentObj) {
                        ParentNode<GameObject_Info> pn;
                        pn.uuid = sel;
                        pn.ptr = parentObj;
                        pn.attached = obj->m_parent.attached; // preserve attach flag
                        obj->SetParent(pn);

                        m_WorldEditor->m_scene->m_sceneGraph->Reparent(obj->m_UUID, sel);
                    }
                }
                currentIndex = i;
            }
            if (selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Attached checkbox (preserve behavior)
    ImGui::SetNextItemWidth(-FLT_MIN);
    bool attached = obj->m_parent.attached;
    ImGui::Checkbox("Attached to parent: ", &attached); ImGui::SameLine();
    if (attached != obj->m_parent.attached)
    {
        if (attached) obj->AttachToParent();
        else obj->DetachFromParent();
    }

}

void PropertyPanel::DrawTransformComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<TransformComponent_Info>()) 
        return;

    auto transform = obj->GetComponent<TransformComponent_Info>()->m_assignedComponent.get();
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | 
                              ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth;
    
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header1);
    if (ImGui::TreeNodeEx("Transform", flags))
    {
        EditorUI::FontStyles::Pop();

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
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawDetails(GameObject_Info* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;


    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header1);
    if (ImGui::TreeNodeEx("Details", flags))
    {
        EditorUI::FontStyles::Pop();

        if (obj->m_group == GameObjectGroup::Lighting)
        {
            switch (obj->m_type.m_asLight)
            {
            case LightObjectType::AmbientLight:
                if (auto lightObj = static_cast<LightObject_Info<SE_G::AmbientLightData>*>(obj))
                {
                    DrawAmbientLightDetails(lightObj->m_lightData.get());
                }
                break;

            case LightObjectType::DirectionalLight:
                if (auto lightObj = static_cast<LightObject_Info<SE_G::DirectionalLightData>*>(obj))
                {
                    DrawDirectionalLightDetails(lightObj->m_lightData.get());
                }
                break;

            case LightObjectType::PointLight:
                if (auto lightObj = static_cast<LightObject_Info<SE_G::PointLightData>*>(obj))
                {
                    DrawPointLightDetails(lightObj->m_lightData.get());
                }
                break;

            case LightObjectType::SpotLight:
                if (auto lightObj = static_cast<LightObject_Info<SE_G::SpotLightData>*>(obj))
                {
                    DrawSpotLightDetails(lightObj->m_lightData.get());
                }
                break;

            case LightObjectType::SkyBox:
                if (auto lightObj = static_cast<SkyBox_Info*>(obj))
                {
                    DrawSkyBoxDetails(lightObj);
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
                if (auto shapeObj = static_cast<BoxShapeObject_Info*>(obj))
                {
                    DrawBoxShapeDetails(shapeObj);
                }
                break;

            case ShapeObjectType::Sphere:
                if (auto shapeObj = static_cast<SphereShapeObject_Info*>(obj))
                {
                    DrawSphereShapeDetails(shapeObj);
                }
                break;

            case ShapeObjectType::Geosphere:
                if (auto shapeObj = static_cast<GeosphereShapeObject_Info*>(obj))
                {
                    DrawGeosphereShapeDetails(shapeObj);
                }
                break;
                
            default:
                break;
            }
        }
        else if (obj->m_group == GameObjectGroup::ParticleEmitter)
        {
            auto emitterObj = static_cast<SE::ParticleEmitter_Info*>(obj);
            DrawEmitterDetails(emitterObj);
        }

        DrawMeshComponent(obj);
        DrawPhysicsComponent(obj);
        DrawLuaComponent(obj);
        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawAmbientLightDetails(SE_G::AmbientLightData* lightData)
{
    if (lightData)
    {

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Ambient Light");
        EditorUI::FontStyles::Pop();

        ImGui::ColorEdit3("Light Color", &lightData->Ambient.x, ImGuiColorEditFlags_Float);
    }
}

void PropertyPanel::DrawDirectionalLightDetails(SE_G::DirectionalLightData* lightData)
{
    if (lightData)
    {
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Directional Light");
        EditorUI::FontStyles::Pop();
        
        ImGui::ColorEdit3("Diffuse Color", &lightData->Diffuse.x, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Specular Color", &lightData->Specular.x, ImGuiColorEditFlags_Float);
        
        float azimut = lightData->Direction.x * (360.0f / DirectX::XM_2PI);
        float height = lightData->Direction.y * (360.0f / DirectX::XM_2PI);
        if (ImGui::DragFloat("Azimut", &azimut, 0.5f, 0.0f, 360.0f, "%.1f m"))
        {
            lightData->Direction.x = azimut * DirectX::XM_2PI / 360.0f;
        }
        if (ImGui::DragFloat("Height", &height, 0.5f, -90.0f, 90.0f, "%.1f m"))
        {
            lightData->Direction.y = height * DirectX::XM_2PI / 360.0f;
        }

    }
}

void PropertyPanel::DrawPointLightDetails(SE_G::PointLightData* lightData)
{
    if (lightData)
    {
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Point Light");
        EditorUI::FontStyles::Pop();
        
        ImGui::ColorEdit3("Diffuse Color", &lightData->Diffuse.x, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Specular Color", &lightData->Specular.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Range", &lightData->Range, 0.5f, 0.0f, 100.0f, "%.1f m");

        ImGui::Text("Attenuation");
        ImGui::DragFloat("1", &lightData->Att.x, 0.5f, 0.0f, 100.0f, "%.1f m");
        ImGui::DragFloat("1/x", &lightData->Att.y, 0.5f, 0.0f, 100.0f, "%.1f m");
        ImGui::DragFloat("1/x2", &lightData->Att.z, 0.5f, 0.0f, 100.0f, "%.1f m");
        //
    }
}

void PropertyPanel::DrawSpotLightDetails(SE_G::SpotLightData* lightData)
{
    if (lightData)
    {
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Spot Light");
        EditorUI::FontStyles::Pop();

        ImGui::ColorEdit3("Diffuse Color", &lightData->Diffuse.x, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Specular Color", &lightData->Specular.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Range", &lightData->Range, 0.5f, 0.0f, 100.0f, "%.1f m");
        ImGui::DragFloat("Spot", &lightData->Spot, 0.1f, 0.0f, 100.0f, "%.1f m");

        ImGui::Text("Attenuation");
        ImGui::DragFloat("1", &lightData->Att.x, 0.5f, 0.0f, 100.0f, "%.1f m");
        ImGui::DragFloat("1/x", &lightData->Att.y, 0.5f, 0.0f, 100.0f, "%.1f m");
        ImGui::DragFloat("1/x2", &lightData->Att.z, 0.5f, 0.0f, 100.0f, "%.1f m");
        /*
        float azimut = lightData->Direction.x * (360.0f / DirectX::XM_2PI);
        float height = lightData->Direction.y * (360.0f / DirectX::XM_2PI);
        if (ImGui::DragFloat("Azimut", &azimut, 0.5f, 0.0f, 360.0f, "%.1f m"))
        {
            lightData->Direction.x = azimut * DirectX::XM_2PI / 360.0f;
        }
        if (ImGui::DragFloat("Height", &height, 0.5f, -90.0f, 90.0f, "%.1f m"))
        {
            lightData->Direction.y = height * DirectX::XM_2PI / 360.0f;
        }
        */
    }
}

void PropertyPanel::DrawSkyBoxDetails(SkyBox_Info* skyBoxObj)
{
    if (skyBoxObj)
    {
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Skybox");
        EditorUI::FontStyles::Pop();
        
        ImGui::ColorEdit3("Sky Tint", &skyBoxObj->m_lightData.get()->Tint.x, ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Intensity", &skyBoxObj->m_lightData.get()->Power, 0.1f, 0.0f, 10.0f, "%.1f");

        ImGui::Separator();
        // Texture

        auto tex = skyBoxObj->m_lightTech->m_texture;

        auto newTexture = DrawTextureSettings(tex);
        if (newTexture)
        {
            newTexture->SetSlot(4u);
            skyBoxObj->SetTexture(newTexture);
        }

        ImGui::Separator();
    }
}

void PropertyPanel::DrawBoxShapeDetails(BoxShapeObject_Info* obj)
{
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    ImGui::Text("Box Shape");
    EditorUI::FontStyles::Pop();
    
    DXSM::Vector3 currentSize = obj->GetSize();
    
    if (DrawVector3Control("Size", currentSize, 1.0f))
    {
        obj->SetSize(m_WorldEditor->m_renderer.get(), currentSize);
    }
}

void PropertyPanel::DrawSphereShapeDetails(SphereShapeObject_Info* obj)
{
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    ImGui::Text("Sphere Shape");
    EditorUI::FontStyles::Pop();
    
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

void PropertyPanel::DrawGeosphereShapeDetails(GeosphereShapeObject_Info* obj)
{
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);    
    ImGui::Text("Geosphere Shape");
    EditorUI::FontStyles::Pop();

    DXSM::Vector3 currentSize = obj->GetSize();
    uint32_t currentNumSubdiv = obj->GetNumSubdivisions();
    
    if (DrawVector3Control("Size", currentSize, 1.0f))
    {
        obj->SetSize(m_WorldEditor->m_renderer.get(), currentSize);
    }

    uint32_t min_numsubdiv = 0, max_numsubdiv = 5;
    if (DrawUIntControl("Subdivisions number", currentNumSubdiv, 6, 1.0f, min_numsubdiv, max_numsubdiv))
    {
        obj->SetNumSubdivisions(m_WorldEditor->m_renderer.get(), currentNumSubdiv);
    }
}

void PropertyPanel::DrawPhysicsComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<PhysicsComponent_Info>()) 
        return;
    if (auto physicsInfo = obj->GetComponent<PhysicsComponent_Info>())
    {   
        ImGui::Separator();

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Physics Settings");
        EditorUI::FontStyles::Pop();
        
        // Button width
        const char* labelRemove = "Remove Physics component";
        ImVec2 textSize = ImGui::CalcTextSize(labelRemove);
        ImVec2 padding = ImGui::GetStyle().FramePadding;
        float labelWidth = textSize.x + padding.x * 2.0f;

        // free space on this line
        ImVec2 avail = ImGui::GetContentRegionAvail();
        avail.x = avail.x - textSize.x;

        if (avail.x > labelWidth) {
            // put on the same line
            float oldX = ImGui::GetCursorPosX();
            ImGui::SameLine();
            ImGui::SetCursorPosX(oldX + avail.x);
        }
        // else dont call SameLine, Button will be under line

        if (ImGui::SmallButton(labelRemove)) {
            obj->RemoveComponent<PhysicsComponent_Info>();
            return;
        }

        auto currentMotion = physicsInfo->GetMotion();
        if (ImGui::Combo("Motion Type", (int*)&currentMotion, "Static\0Kinematic\0Dynamic\0"))
        {
            physicsInfo->SetMotion(currentMotion);
        }

        auto currentActivation = physicsInfo->GetActivation();
        if (ImGui::Combo("Activation", (int*)&currentActivation, "Activate\0Don't Activate\0"))
        {
            physicsInfo->SetActivation(currentActivation);
        }

        auto currentLayer = physicsInfo->GetCollisionLayer();
        int layerIndex = currentLayer == "MOVING"? 1 : 0;
        const char* layerItems = "Non Moving\0Moving\0";
        
        if (ImGui::Combo("Collision Layer", &layerIndex, layerItems))
        {
            physicsInfo->SetCollisionLayer(layerIndex == 0 ? "NON_MOVING" : "MOVING");
        }
        
        if (auto colliderData = physicsInfo->m_colliderData) {
            ImGui::Separator();

            EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
            ImGui::Text("Collider Settings");
            EditorUI::FontStyles::Pop();

            auto currentShape = colliderData->GetShapeType();
            const char* shapeItems = 
                "Sphere\0Box\0Capsule\0TaperedCapsule\0";
            if (ImGui::Combo("Shape Type", (int*)&currentShape, shapeItems))
            {
                colliderData->SetShapeType(currentShape);
            }
            
            /*
            auto transformData = colliderData->GetTransformData();
            bool transformChanged = false;
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | 
                              ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth;
            if (ImGui::TreeNodeEx("Collider Transform", flags))
            {
                if (DrawVector3Control("Offset", transformData.m_offset, 0.0f))
                    transformChanged = true;
                
                DXSM::Vector3 rotationDeg = transformData.m_rotation * (180.0f / DirectX::XM_PI);
                if (DrawVector3Control("Rotation", rotationDeg, 0.0f))
                {
                    transformData.m_rotation = rotationDeg * (DirectX::XM_PI / 180.0f);
                    transformChanged = true;
                }
        
                ImGui::TreePop();
            }
            if (transformChanged)
            {
                colliderData->SetTransformData(transformData);
            }
            */

            auto settings = colliderData->GetColliderSettings();
            bool settingsChanged = false;
    
            switch (colliderData->GetShapeType())
            {
            case SE::ColliderShapeType::Box:
                if (DrawVector3Control("Box Size", settings.data.asBox.m_size, 1.0f))
                    settingsChanged = true;
                break;
        
            case SE::ColliderShapeType::Sphere:
                if (DrawFloatControl("Radius", settings.data.asSphere.m_radius, 1.0f, 0.1f, 0.01f, 100.0f, "%.2f"))
                    settingsChanged = true;
                break;
        
            case SE::ColliderShapeType::Capsule:
                if (DrawFloatControl("Height", settings.data.asCapsule.m_height, 1.0f, 0.1f, 0.01f, 100.0f, "%.2f"))
                    settingsChanged = true;
                if (DrawFloatControl("Radius", settings.data.asCapsule.m_radius, 1.0f, 0.1f, 0.01f, 100.0f, "%.2f"))
                    settingsChanged = true;
                break;
        
            case SE::ColliderShapeType::TaperedCapsule:
                if (DrawFloatControl("Height", settings.data.asTaperedCapsule.m_height, 1.0f, 0.1f, 0.01f, 100.0f, "%.2f"))
                    settingsChanged = true;
                if (DrawFloatControl("Top Radius", settings.data.asTaperedCapsule.m_topRadius, 1.0f, 0.1f, 0.01f, 100.0f, "%.2f"))
                    settingsChanged = true;
                if (DrawFloatControl("Bottom Radius", settings.data.asTaperedCapsule.m_bottomRadius, 1.0f, 0.1f, 0.01f, 100.0f, "%.2f"))
                    settingsChanged = true;
                break;
        
            case SE::ColliderShapeType::Mesh:
                ImGui::Text("Using mesh");
                break;

            case SE::ColliderShapeType::Cylinder:
                ImGui::Text("Using cylinder");
                break;
                
            case SE::ColliderShapeType::TaperedCylinder:
                ImGui::Text("Using tapered cylinder");
                break;

            case SE::ColliderShapeType::Plane:
                ImGui::Text("Using plane");
                break;
                
            case SE::ColliderShapeType::Triangle:
                ImGui::Text("Using triangle");
                break;
                
            case SE::ColliderShapeType::Empty:
                ImGui::Text("Using empty");
                break;

            case SE::ColliderShapeType::ConvexHull:
                ImGui::Text("Using convex hull");
                break;
                
            case SE::ColliderShapeType::HeightField:
                ImGui::Text("Using height field");
                break;

            case SE::ColliderShapeType::SoftBody:
                ImGui::Text("Using soft body");
                break;

            case SE::ColliderShapeType::StaticCompound:
                ImGui::Text("Using static compound");
                break;
                
            case SE::ColliderShapeType::MutableCompound:
                ImGui::Text("Using mutable compound");
                break;

            case SE::ColliderShapeType::Scaled:
                ImGui::Text("Using scaled");
                break;
                
            case SE::ColliderShapeType::RotatedTranslated:
                ImGui::Text("Using rotated translated");
                break;
                
            case SE::ColliderShapeType::OffsetCenterOfMass:
                ImGui::TextDisabled("Using offset center of mass");
                break;
            }
    
            if (settingsChanged)
            {
                colliderData->SetColliderSettings(settings);
            }
        }
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


    if (auto luaInfo = obj->GetComponent<LuaComponent_Info>())
    {
        ImGui::Separator();

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Lua Settings");
        EditorUI::FontStyles::Pop();

        // Button width
        const char* labelRemove = "Remove";
        ImVec2 textSize = ImGui::CalcTextSize(labelRemove);
        ImVec2 padding = ImGui::GetStyle().FramePadding;
        float labelWidth = textSize.x + padding.x * 2.0f;

        // free space on this line
        ImVec2 avail = ImGui::GetContentRegionAvail();
        avail.x = avail.x - textSize.x;

        if (avail.x > labelWidth) {
            // put on the same line
            float oldX = ImGui::GetCursorPosX();
            ImGui::SameLine();
            ImGui::SetCursorPosX(oldX + avail.x);
        }
        // else dont call SameLine, Button will be under line

        if (ImGui::SmallButton(labelRemove)) {
            obj->RemoveComponent<LuaComponent_Info>();
            return;
        }

        uint32_t luaScriptIndex = luaInfo->selectedLuaFile;
        if (DrawUIntControl("Lua Script Index", luaScriptIndex, 10, 1.0f))
        {
            luaInfo->selectedLuaFile = luaScriptIndex;
            luaInfo->InitLuaFile();
            ImGui::Text(luaInfo->scriptPath.c_str());
        }        

    }
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

        if (!obj->HasComponent<PhysicsComponent_Info>())
        {
            if (ImGui::MenuItem("Physics Component", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::PHYSICS);
            }
        }
        if (!obj->HasComponent<MeshComponent_Info>())
        {
            if (ImGui::MenuItem("Mesh Component", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::MESH);
            }
        }

        
        if (!obj->HasComponent<LuaComponent_Info>())
        {
            if (ImGui::MenuItem("Lua Script", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::LUA);
            }
        }
        else
        {
            ImGui::TextDisabled("All available components added");
        }
        
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

void PropertyPanel::DrawMeshComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<MeshComponent_Info>())
        return;
    
    auto meshInfo = obj->GetComponent<MeshComponent_Info>();

    ImGui::Separator();

    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    ImGui::Text("Mesh");
    EditorUI::FontStyles::Pop();

    if (!(obj->m_group == GameObjectGroup::Shapes))
    {
        // Button width calculation similar to other components
        const char* labelRemove = "Remove Mesh component";
        ImVec2 textSize = ImGui::CalcTextSize(labelRemove);
        ImVec2 padding = ImGui::GetStyle().FramePadding;
        float labelWidth = textSize.x + padding.x * 2.0f;

        ImVec2 avail = ImGui::GetContentRegionAvail();
        avail.x = avail.x - textSize.x;

        if (avail.x > labelWidth) {
            // put on the same line
            float oldX = ImGui::GetCursorPosX();
            ImGui::SameLine();
            ImGui::SetCursorPosX(oldX + avail.x);
        }

        if (ImGui::SmallButton(labelRemove)) {
            obj->RemoveComponent<MeshComponent_Info>();
            return; // component removed, nothing to draw
        }
    }

    // Draw mesh info
    if (!meshInfo->IsAssigned()) {
        ImGui::TextDisabled("No mesh assigned");
        return;
    }

    auto assigned = meshInfo->m_assignedComponent.get();
    if (!assigned) {
        ImGui::TextDisabled("(mesh component not initialized)");
        return;
    }

    auto meshPtr = assigned->GetMesh();
    auto newMesh = DrawMeshSettings(meshPtr, obj->m_group);
    if (newMesh)
    {
        assigned->SetMesh(newMesh);
    }

    ImGui::Separator();
    // Texture
    auto tex = assigned->GetTexture();

    auto newTexture = DrawTextureSettings(tex);
    if (newTexture)
    {
        newTexture->SetSlot(0u);
        assigned->SetTexture(newTexture);
    }
    ImGui::Separator();

    // Sampler
    auto sampler = assigned->GetTextureSamplerPreset();

    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
    ImGui::Text("Sampler settings");
    EditorUI::FontStyles::Pop();

    if (sampler) {
        auto preset = sampler->GetPreset();
        const char* presetName = "Unknown";
        switch (preset) {
        case SE_G::Bind::SamplerPreset::Wrap: presetName = "Wrap"; break;
        case SE_G::Bind::SamplerPreset::Mirror: presetName = "Mirror"; break;
        case SE_G::Bind::SamplerPreset::Clamp: presetName = "Clamp"; break;
        case SE_G::Bind::SamplerPreset::Border: presetName = "Border"; break;
        }
        ImGui::Text("Sampler: %s", presetName);
    } else {
        ImGui::TextDisabled("Sampler: (none)");
    }
}

void PropertyPanel::DrawEmitterDetails(
    SE::ParticleEmitter_Info* emitterObj
    /*
    SE::ParticleData::EmitterPointConstantBuffer* emitterPointBuffer,
    SE::ParticleData::SimulateParticlesConstantBuffer* simulateParticlesBuffer
    */
    )
{
    if (emitterObj)
    {
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Particle Emitter");
        EditorUI::FontStyles::Pop();

        auto emitterData = emitterObj->m_particleData.get();
        auto emitterPointBuffer = &emitterData->m_emitterConstantBufferData;
        auto simulateParticlesBuffer = &emitterData->m_simulateParticlesConstantBufferData;

        // Enable button
        if (ImGui::SmallButton(emitterData->m_enabled ? "Hide particles" : "Show particles")) {
            emitterData->m_enabled = !emitterData->m_enabled;
            if (emitterData->m_enabled)
            {
                emitterData->EnableEmission();
            }
            else
            {
                emitterData->DisableEmission();
            }
        }


        DrawVector3Control("Position", emitterPointBuffer->position, 0.0f);

        ImGui::ColorEdit3("Color start", &emitterPointBuffer->colorStart.x, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Color end", &emitterPointBuffer->colorEnd.x, ImGuiColorEditFlags_Float);

        float alphaStart = emitterPointBuffer->alphaStart * 255.0f;
        if (ImGui::DragFloat("Alpha start", &alphaStart,
            1.0f, 0.0f, 255.0f, "%.1f"))
        {
            emitterPointBuffer->alphaStart = alphaStart / 255.0f;
        }

        float alphaEnd = emitterPointBuffer->alphaEnd * 255.0f;
        if (ImGui::DragFloat("Alpha end", &alphaEnd,
            1.0f, 0.0f, 255.0f, "%.1f"))
        {
            emitterPointBuffer->alphaEnd = alphaEnd / 255.0f;
        }

        uint32_t min_count = 3, max_count = 64;
        if (DrawUIntControl("Max particles count", emitterData->m_maxParticles, 0u, 1u, min_count, max_count))
        {
        }

        ImGui::DragFloat("Emission rate", &emitterData->m_deaultEmissionRate,
            0.1f, 0.0f, 100.0f, "%.1f");

        ImGui::DragFloat("Particles lifetime", &emitterPointBuffer->particlesLifeSpan,
            0.1f, 0.1f, 10.0f, "%.1f sec");

        ImGui::DragFloat("Particles base speed", &emitterPointBuffer->particlesBaseSpeed,
            0.1f, 0.0f, 20.0f, "%.1f m/s");

        ImGui::DragFloat("Particles mass", &emitterPointBuffer->particlesMass,
            0.1f, 0.0f, 10.0f, "%.1f");

        ImGui::DragFloat("Particles start size", &emitterPointBuffer->particleSizeStart,
            0.1f, 0.0f, 10.0f, "%.1f m");

        ImGui::DragFloat("Particles end size", &emitterPointBuffer->particleSizeEnd,
            0.1f, 0.0f, 10.0f, "%.1f m");

        float longitudeMin = emitterPointBuffer->longitudeMin * (180.0f / DirectX::XM_PI);
        ImGui::Text("Random longitude range:");
        if (ImGui::DragFloat("Longitude min", &longitudeMin, 0.1f,
            0.0f, emitterPointBuffer->longitudeMax * (180.0f / DirectX::XM_PI), "%.1f"))
        {
            emitterPointBuffer->longitudeMin = longitudeMin * (DirectX::XM_PI / 180.0f);
        }
        float longitudeMax = emitterPointBuffer->longitudeMax * (180.0f / DirectX::XM_PI);
        if (ImGui::DragFloat("Longitude max", &longitudeMax, 0.1f,
            emitterPointBuffer->longitudeMin * (180.0f / DirectX::XM_PI), 360.0f, "%.1f"))
        {
            emitterPointBuffer->longitudeMax = longitudeMax * (DirectX::XM_PI / 180.0f);
        }

        float latitudeMin = emitterPointBuffer->latitudeMin * (180.0f / DirectX::XM_PI);
        ImGui::Text("Random latitude range:");
        if (ImGui::DragFloat("Latitude min", &latitudeMin, 0.1f,
            -90.0f, emitterPointBuffer->latitudeMax * (180.0f / DirectX::XM_PI), "%.1f"))
        {
            emitterPointBuffer->latitudeMin = latitudeMin * (DirectX::XM_PI / 180.0f);
        }
        float latitudeMax = emitterPointBuffer->latitudeMax * (180.0f / DirectX::XM_PI);
        if (ImGui::DragFloat("Latitude max", &latitudeMax, 0.1f,
            emitterPointBuffer->latitudeMin * (180.0f / DirectX::XM_PI), 90.0f, "%.1f"))
        {
            emitterPointBuffer->latitudeMax = latitudeMax * (DirectX::XM_PI / 180.0f);
        }

        ImGui::Separator();

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        ImGui::Text("Particle force");
        EditorUI::FontStyles::Pop();

        DrawVector3Control("Force vector", simulateParticlesBuffer->force, 0.0f);

        ImGui::Separator();
        // Texture
        
        auto tex = emitterObj->m_particleData->m_texture;

        auto newTexture = DrawTextureSettings(tex);
        if (newTexture)
        {
            newTexture->SetSlot(0u);
            emitterObj->m_particleData->SetTexture(newTexture);
        }

        ImGui::Separator();
    }
}

eastl::shared_ptr<SE_G::Bind::Texture> PropertyPanel::DrawTextureSettings(
    eastl::shared_ptr<SE_G::Bind::Texture> texture)
{
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
    ImGui::Text("Texture settings");
    EditorUI::FontStyles::Pop();

    if (texture) {
        eastl::wstring tpath = texture->GetCurrentTexturePath().m_assetRelativePath;
        // convert wstring to narrow string for ImGui display
        if (texture->GetCurrentTexturePath().m_assetSource == AssetPath::AssetSource::Engine)
        {
            ImGui::TextDisabled("Engine asset");
        }
        else
        {
            ImGui::TextDisabled("Project asset");
        }

        std::wstring ws = tpath.c_str();
        std::string s(ws.begin(), ws.end());
        ImGui::Text("Texture: %s", s.c_str());
    }
    else {
        ImGui::TextDisabled("Texture: (none)");
    }

    // Editing button
    if (ImGui::SmallButton(s_meshEditor.m_editTexture ? "Close Texture Editor" : "Edit Texture")) {
        s_meshEditor.m_editTexture = !s_meshEditor.m_editTexture;
        s_meshEditor.m_texError.clear();
        // �����������: ��� �������� ��������� ���� �������� ����������
        if (s_meshEditor.m_editTexture && texture) {
            AssetPath cur = texture->GetCurrentTexturePath();
            std::wstring ws = cur.m_assetRelativePath.c_str();
            std::string  s(ws.begin(), ws.end());
            strncpy(s_meshEditor.m_texPathBuf, s.c_str(), sizeof(s_meshEditor.m_texPathBuf) - 1);
            s_meshEditor.m_texPathBuf[sizeof(s_meshEditor.m_texPathBuf) - 1] = 0;
            s_meshEditor.m_texAssetSource = cur.m_assetSource;
        }
    }
    eastl::shared_ptr<SE_G::Bind::Texture> newTexture;
    // Editing panel
    if (s_meshEditor.m_editTexture) {
        ImGui::Separator();

        ImGui::InputText("Texture asset path", s_meshEditor.m_texPathBuf, sizeof(s_meshEditor.m_texPathBuf));

        const char* srcItems = "Engine\0Project\0";
        ImGui::Combo("Texture Source", (int*)&s_meshEditor.m_texAssetSource, srcItems);

        if (ImGui::Button("Load Texture")) {
            s_meshEditor.m_texError.clear();

            AssetPath::AssetSource src = s_meshEditor.m_texAssetSource;
            std::string relNarrow = s_meshEditor.m_texPathBuf;
            std::wstring relWide(relNarrow.begin(), relNarrow.end());
            AssetPath ap(relWide.c_str(), src);
            // Without Resource manager
            /*
            {
                newTexture = eastl::make_shared<SE_G::Bind::Texture>(m_WorldEditor->m_renderer->GetDevice(), ap, 4u);
                // skyBoxObj->SetTexture(newTexture);
                
            }
            */
            // Using Resource manager
            ResourceHandle texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);

            if (texHandle.guid == 0) {
                //s_meshEditor.m_texError = "Failed to load texture: " + relNarrow;
            }
            else {
                SE_G::Bind::Texture* texture =
                    ResourceManagerFacade::Instance().Get<SE_G::Bind::Texture>(texHandle);

                if (texture) {
                    newTexture = eastl::shared_ptr<SE_G::Bind::Texture>(
                        texture,
                        [](SE_G::Bind::Texture*) {}
                    );
                    newTexture->m_texturePath = texture->m_texturePath;
                }
                else {
                    s_meshEditor.m_texError = "Failed to cast loaded resource to Texture";
                }
            }
            s_meshEditor.m_editTexture = false;
        }

        if (!s_meshEditor.m_texError.empty()) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Texture load error: %s", s_meshEditor.m_texError.c_str());
        }
    }
    return newTexture;
}

eastl::shared_ptr<SE_G::Mesh> PropertyPanel::DrawMeshSettings(
    eastl::shared_ptr<SE_G::Mesh> meshPtr, GameObjectGroup group)
{
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
    ImGui::Text("Mesh settings");
    EditorUI::FontStyles::Pop();

    if (meshPtr) {
        eastl::wstring mpath = meshPtr->GetCurrentMeshPath().m_assetRelativePath;
        // convert wstring to narrow string for ImGui display
        std::wstring ws = mpath.c_str();
        std::string s(ws.begin(), ws.end());
        if (meshPtr->GetCurrentMeshPath().m_assetSource == AssetPath::AssetSource::Engine)
        {
            ImGui::TextDisabled("Engine asset");
        }
        else
        {
            ImGui::TextDisabled("Project asset");
        }
        ImGui::Text("Mesh: %s", s.c_str());
    }
    else {
        ImGui::TextDisabled("Mesh: (procedural or empty)");
    }

    eastl::shared_ptr<SE_G::Mesh> newMesh;
    if (!(group == GameObjectGroup::Shapes))
    {
        // Editing button
        if (ImGui::SmallButton(s_meshEditor.m_editMesh ? "Close Mesh Editor" : "Edit Mesh")) {
            s_meshEditor.m_editMesh = !s_meshEditor.m_editMesh;
            s_meshEditor.m_meshError.clear();
            // �����������: ��� �������� ��������� ���� �������� ����������
            if (s_meshEditor.m_editMesh && meshPtr) {
                AssetPath cur = meshPtr->GetCurrentMeshPath();
                std::wstring ws = cur.m_assetRelativePath.c_str();
                std::string  s(ws.begin(), ws.end());
                strncpy(s_meshEditor.m_meshPathBuf, s.c_str(), sizeof(s_meshEditor.m_meshPathBuf) - 1);
                s_meshEditor.m_meshPathBuf[sizeof(s_meshEditor.m_meshPathBuf) - 1] = 0;
                s_meshEditor.m_meshAssetSource = cur.m_assetSource;
            }
        }

        // Editing panel
        if (s_meshEditor.m_editMesh) {
            ImGui::Separator();

            ImGui::InputText("Mesh asset path", s_meshEditor.m_meshPathBuf, sizeof(s_meshEditor.m_meshPathBuf));

            const char* srcItems = "Engine\0Project\0";
            ImGui::Combo("Mesh Source", (int*)&s_meshEditor.m_meshAssetSource, srcItems);

            if (ImGui::Button("Load Mesh")) {
                s_meshEditor.m_meshError.clear();

                AssetPath::AssetSource src = s_meshEditor.m_meshAssetSource;

                std::string relNarrow = s_meshEditor.m_meshPathBuf;
                std::wstring relWide(relNarrow.begin(), relNarrow.end());

                AssetPath ap(relWide.c_str(), src);

                {
                    newMesh = eastl::make_shared<SE_G::Mesh>(m_WorldEditor->m_renderer->GetDevice(), ap);
                    //assigned->SetMesh(newMesh);

                }

                s_meshEditor.m_editMesh = false;
            }

            if (!s_meshEditor.m_meshError.empty()) {
                ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Mesh load error: %s", s_meshEditor.m_meshError.c_str());
            }
        }
    }
    else
    {
        ImGui::BeginDisabled();
        ImGui::Button("Edit Mesh");
        ImGui::SetItemTooltip("Mesh editor disabled for shapes");
        ImGui::EndDisabled();
    }

    return newMesh;
}