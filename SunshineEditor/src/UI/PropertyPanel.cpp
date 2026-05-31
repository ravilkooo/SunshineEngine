#include "DirectXMath.h"

#include "WorldEditor.h"

#include <Graphics/Renderer/MiniViewRenderer.h>
#include <Graphics/Renderer/GBuffer.h>
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

#include <ParticleSystem/ParticleEmitterComponent.h>

#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/MeshComponent.h>
#include <Component/LuaComponent.h>
#include <Component/TriggerComponent.h>
#include <Component/CharacterComponent.h>
#include <Component/CameraComponent.h>
#include <Component/CharacterControllerComponent.h>

#include "AI/Perception/PerceptionComponent.h"
#include "AI/Behavior/BehaviorController.h"

#include <Scene.h>
#include <SceneHierarchy.h>

#include <CameraManager.h>

#include <UI/PropertyPanel.h>
#include <UI/FontStyles.h>
#include "UI/ContentBrowserPanel.h"
#include <UI/ImguiUtils.h>

#include "Audio/AudioEditor.h"
#include "Audio/AudioSystem.h"
#include "Utils/FileDialogManager.h"

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
        if (m_WorldEditor && m_WorldEditor->m_scene && m_WorldEditor->m_scene->m_sceneGraph)
        {
            SE::UUID objUUID = obj->m_UUID;
            
            // Collect all UUIDs in the subtree (including root)
            eastl::vector<SE::UUID> toRemove;
            eastl::vector<SE::UUID> stack;
            stack.push_back(objUUID);
            
            while (!stack.empty())
            {
                SE::UUID current = stack.back();
                stack.pop_back();
                toRemove.push_back(current);
                
                // Find node in scene graph
                auto it = m_WorldEditor->m_scene->m_sceneGraph->m_byObjUUID.find(current);
                if (it != m_WorldEditor->m_scene->m_sceneGraph->m_byObjUUID.end())
                {
                    int nodeIdx = it->second;
                    const auto& node = m_WorldEditor->m_scene->m_sceneGraph->m_nodes[nodeIdx];
                    
                    // Add all children to stack
                    for (SE::UUID childUUID : node.children)
                    {
                        stack.push_back(childUUID);
                    }
                }
            }
            
            // Remove from scene hierarchy first
            m_WorldEditor->m_scene->m_sceneGraph->EraseSubtree(objUUID);
            
            // Remove all collected objects from scene
            for (SE::UUID uuid : toRemove)
            {
                m_WorldEditor->m_scene->RemoveGameObjectByUUID(uuid);
            }
            
            // Clear selection
            m_WorldEditor->m_hierarchySelection.picked.clear();
            m_WorldEditor->m_hierarchySelection.last_clicked = SE::UUID(0u);
            m_WorldEditor->m_selectionPass->m_selectedObjectUUID = SE::UUID(0u);
            m_SelectedUUID = SE::UUID(0u);
            return false;
        }
    }

    auto uuidhilo = obj->m_UUID.GetHilo();
    ImGui::TextDisabled("UUID (hi,lo): (%lu, %lu)", uuidhilo.hi, uuidhilo.lo);
	// ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "UUID (hi,lo): (%lu, %lu)", uuidhilo.hi, uuidhilo.lo);

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
    if (obj->m_parent.uuid != SE::UUID(0u) && obj->m_parent.ptr)
    {
        ImGui::SetNextItemWidth(-FLT_MIN);
        bool attached = obj->m_parent.attached;
        ImGui::Checkbox("Attached to parent: ", &attached); ImGui::SameLine();
        if (attached != obj->m_parent.attached)
        {
            if (attached) obj->AttachToParent();
            else obj->DetachFromParent();
        }
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
    if (ImGui::TreeNodeEx("Object Transform", flags))
    {
        EditorUI::FontStyles::Pop();

        ImguiUtils::DrawVector3Control("Position", transform->m_position,
            DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
            DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
            0.0f);

        DXSM::Vector3 rotationDeg = transform->m_rotation * (180.0f / DirectX::XM_PI);
        if (ImguiUtils::DrawVector3Control("Rotation", rotationDeg,
            DXSM::Vector3(-360.0f, -360.0f, -360.0f),
            DXSM::Vector3(360.0f, 360.0f, 360.0f),
            0.0f))
        {
            transform->m_rotation = rotationDeg * (DirectX::XM_PI / 180.0f);
        }

        ImguiUtils::DrawVector3Control("Scale", transform->m_scaleFactor,
            DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
            DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
            1.0f);

        ImguiUtils::DrawVector2Control("UV Multiplier", transform->m_uvMultiplier, 1.0f);

        /*
        if (ImGui::TreeNode("Local Transform"))
        {
            ImguiUtils::DrawVector3Control("Local Position", transform->m_localPosition, 0.0f);

            DXSM::Vector3 localRotDeg = transform->m_localRotation * (180.0f / DirectX::XM_PI);
            if (ImguiUtils::DrawVector3Control("Local Rotation", localRotDeg, 0.0f))
            {
                transform->m_localRotation = localRotDeg * (DirectX::XM_PI / 180.0f);
            }

            ImguiUtils::DrawVector3Control("Local Scale", transform->m_localScaleFactor, 1.0f);

            ImGui::TreePop();
        }
        */

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

            case ShapeObjectType::Cylinder:
                if (auto shapeObj = static_cast<CylinderShapeObject_Info*>(obj))
                {
                    DrawCylinderShapeDetails(shapeObj);
                }
                break;

            default:
                break;
            }
        }
        else if (obj->m_group == GameObjectGroup::ParticleEmitter)
        {
            DrawEmitterDetails(obj);
        }

        DrawMeshComponent(obj);
        DrawCharacterComponent(obj);
        DrawCharacterControllerComponent(obj);
        DrawCameraComponent(obj);
        DrawPhysicsComponent(obj);
        DrawTriggerComponent(obj);
        DrawPerceptionComponent(obj);
        DrawBehaviorController(obj);
        DrawLuaComponent(obj);

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawAmbientLightDetails(SE_G::AmbientLightData* lightData)
{
    if (lightData)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        ImGui::Separator();
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        if (ImGui::TreeNodeEx("Ambient Light", flags))
        {
            EditorUI::FontStyles::Pop();
            ImGui::ColorEdit3("Light Color", &lightData->Ambient.x, ImGuiColorEditFlags_Float);
            ImGui::TreePop();
        }
        else EditorUI::FontStyles::Pop();

    }
}

void PropertyPanel::DrawDirectionalLightDetails(SE_G::DirectionalLightData* lightData)
{
    if (lightData)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        ImGui::Separator();
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        if (ImGui::TreeNodeEx("Directional Light", flags))
        {
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
            ImGui::TreePop();
        }
        else EditorUI::FontStyles::Pop();

    }
}

void PropertyPanel::DrawPointLightDetails(SE_G::PointLightData* lightData)
{
    if (lightData)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        ImGui::Separator();
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        if (ImGui::TreeNodeEx("Point Light", flags))
        {
            EditorUI::FontStyles::Pop();

            ImGui::ColorEdit3("Diffuse Color", &lightData->Diffuse.x, ImGuiColorEditFlags_Float);
            ImGui::ColorEdit3("Specular Color", &lightData->Specular.x, ImGuiColorEditFlags_Float);
            ImGui::DragFloat("Range", &lightData->Range, 0.5f, 0.0f, 100.0f, "%.1f m");

            ImGui::Text("Attenuation");
            ImGui::DragFloat("1", &lightData->Att.x, 0.5f, 0.0f, 100.0f, "%.1f m");
            ImGui::DragFloat("1/x", &lightData->Att.y, 0.5f, 0.0f, 100.0f, "%.1f m");
            ImGui::DragFloat("1/x2", &lightData->Att.z, 0.5f, 0.0f, 100.0f, "%.1f m");
            ImGui::TreePop();
        }
        else EditorUI::FontStyles::Pop();
    }
}

void PropertyPanel::DrawSpotLightDetails(SE_G::SpotLightData* lightData)
{
    if (lightData)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        ImGui::Separator();
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        if (ImGui::TreeNodeEx("Spot Light", flags))
        {
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
            ImGui::TreePop();
        }
        else EditorUI::FontStyles::Pop();
    }
}

void PropertyPanel::DrawSkyBoxDetails(SkyBox_Info* skyBoxObj)
{
    if (skyBoxObj)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        ImGui::Separator();
        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
        if (ImGui::TreeNodeEx("Skybox", flags))
        {
            EditorUI::FontStyles::Pop();

            ImGui::ColorEdit3("Sky Tint", &skyBoxObj->m_lightData.get()->Tint.x, ImGuiColorEditFlags_Float);
            ImGui::DragFloat("Intensity", &skyBoxObj->m_lightData.get()->Power, 0.1f, 0.0f, 10.0f, "%.1f");

            ImGui::Separator();
            // Texture

            auto tex = skyBoxObj->m_lightTech->m_texture;

            auto newTexture = DrawTextureSettings(tex, "SkyBox");
            if (newTexture)
            {
                newTexture->SetSlot(4u);
                skyBoxObj->SetTexture(newTexture);
            }
            ImGui::Separator();
            ImGui::TreePop();
        }
        else EditorUI::FontStyles::Pop();
    }
}

void PropertyPanel::DrawBoxShapeDetails(BoxShapeObject_Info* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Box Shape", flags))
    {
        EditorUI::FontStyles::Pop();

        bool uvCubeMapMode = obj->IsUvCubeMapMode();

        ImGui::Checkbox("UV Cubemap Mode", &uvCubeMapMode);
        if (uvCubeMapMode != obj->IsUvCubeMapMode())
        {
            obj->SetUvCubeMapMode(m_WorldEditor->m_renderer.get(), uvCubeMapMode);
        }
        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawSphereShapeDetails(SphereShapeObject_Info* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Sphere Shape", flags))
    {
        EditorUI::FontStyles::Pop();

        uint32_t currentSliceCount = obj->GetSliceCount();
        uint32_t currentStackCount = obj->GetStackCount();

        uint32_t min_slice = 3, max_slice = 40;
        if (DrawUIntControl("Slice Count", currentSliceCount, 10, 1.0f, min_slice, max_slice))
        {
            obj->SetSliceCount(m_WorldEditor->m_renderer.get(), currentSliceCount);
        }

        uint32_t min_stack = 3, max_stack = 40;
        if (DrawUIntControl("Stack Count", currentStackCount, 10, 1.0f, min_stack, max_stack))
        {
            obj->SetStackCount(m_WorldEditor->m_renderer.get(), currentStackCount);
        }
        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawGeosphereShapeDetails(GeosphereShapeObject_Info* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Geosphere Shape", flags))
    {
        EditorUI::FontStyles::Pop();

        uint32_t currentNumSubdiv = obj->GetNumSubdivisions();

        uint32_t min_numsubdiv = 0, max_numsubdiv = 5;
        if (DrawUIntControl("Subdivisions number", currentNumSubdiv, 6, 1.0f, min_numsubdiv, max_numsubdiv))
        {
            obj->
                SetNumSubdivisions(m_WorldEditor->m_renderer.get(), currentNumSubdiv);
        }
        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawCylinderShapeDetails(CylinderShapeObject_Info* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Cylinder Shape", flags))
    {
        EditorUI::FontStyles::Pop();

        uint32_t currentSliceCount = obj->GetSliceCount();

        uint32_t min_slice = 3, max_slice = 40;
        if (DrawUIntControl("Slice Count", currentSliceCount, 10, 1.0f, min_slice, max_slice))
        {
            obj->SetSliceCount(m_WorldEditor->m_renderer.get(), currentSliceCount);
        }

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawPhysicsComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<PhysicsComponent_Info>())
        return;
    auto physicsInfo = obj->GetComponent<PhysicsComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Physics Settings", flags))
    {
        EditorUI::FontStyles::Pop();

        if (DrawComponentRemoveButton<PhysicsComponent_Info>(obj))
        {
            ImGui::TreePop();
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
        int layerIndex = currentLayer == "MOVING" ? 1 : 0;
        const char* layerItems = "Non Moving\0Moving\0";

        if (ImGui::Combo("Collision Layer", &layerIndex, layerItems))
        {
            physicsInfo->SetCollisionLayer(layerIndex == 0 ? "NON_MOVING" : "MOVING");
        }
        ImGui::Separator();


        if (auto colliderData = physicsInfo->m_colliderData)
        {
            DrawColliderSettings(colliderData);
        }


        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
        if (ImGui::TreeNodeEx("Additional Forces", flags))
        {
            EditorUI::FontStyles::Pop();
            DrawFloatControl("Friction", physicsInfo->m_friction, 0.2f, 0.01f, 0.0f, 5.0f, "%.2f");
            ImGui::SetItemTooltip(
                "Static friction between surfaces\n\n"
                "0.0 = Ice (very slippery)\n"
                "0.5 = Metal\n"
                "1.0 = Rubber/Floor (grippy)\n"
                "1.5+ = Very sticky\n\n"
                "Higher = less sliding"
            );

            DrawFloatControl("Linear Damping", physicsInfo->m_linearDamping, 0.05f, 0.001f, 0.0f, 1.0f, "%.3f");
            ImGui::SetItemTooltip(
                "Air resistance (linear)\n\n"
                "0.00 = Space (no resistance)\n"
                "0.05 = Air\n"
                "0.30 = Water\n"
                "0.80+ = Thick medium\n\n"
                "Higher = stops faster"
            );

            DrawFloatControl("Angular Damping", physicsInfo->m_angularDamping, 0.05f, 0.001f, 0.0f, 1.0f, "%.3f");
            ImGui::SetItemTooltip(
                "Rotation resistance\n\n"
                "0.00 = Spins forever\n"
                "0.10 = Air\n"
                "0.50 = Medium\n"
                "0.90+ = Stops quickly\n\n"
                "Prevents endless spinning"
            );

            DrawFloatControl("Restitution", physicsInfo->m_restitution, 0.0f, 0.01f, 0.0f, 10.0f, "%.2f");
            ImGui::SetItemTooltip(
                "Bounciness (coefficient of restitution)\n\n"
                "0.0 = Sticks, no bounce\n"
                "0.3 = Concrete\n"
                "0.7 = Rubber ball\n"
                "1.0 = Superball (perfect bounce)\n\n"
                "Higher = more bounce on collision"
            );

            ImGui::TreePop();
        }
        else EditorUI::FontStyles::Pop();

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawTriggerComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<TriggerComponent_Info>())
        return;
    auto triggerInfo = obj->GetComponent<TriggerComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Trigger Settings", flags))
    {
        EditorUI::FontStyles::Pop();

        if (DrawComponentRemoveButton<TriggerComponent_Info>(obj))
        {
            ImGui::TreePop();
            return;
        }

        if (auto colliderData = triggerInfo->m_colliderData)
        {
            DrawColliderSettings(colliderData);
        }

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawLuaComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<LuaComponent_Info>())
        return;

    auto luaInfo = obj->GetComponent<LuaComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Lua Settings", flags))
    {
        EditorUI::FontStyles::Pop();

        if (DrawComponentRemoveButton<LuaComponent_Info>(obj))
        {
            ImGui::TreePop();
            return;
        }

        /*
        uint32_t luaScriptIndex = luaInfo->selectedLuaFile;
        if (DrawUIntControl("Lua Script Index", luaScriptIndex, 10, 1.0f))
        {
            luaInfo->selectedLuaFile = luaScriptIndex;
            luaInfo->InitLuaFile();
            ImGui::Text(luaInfo->audioPath.c_str());
        }
        */

        LuaComponent_Info::ScanLuaFiles();
        eastl::vector<eastl::string> comboItems;
        comboItems.reserve(LuaComponent_Info::luaFiles.size() + 1);

        // First entry = None
        comboItems.push_back(eastl::string("None"));

        int currentIndex = 0; // default to None
        for (int i = 0; i < (int)LuaComponent_Info::luaFiles.size(); ++i) {
            AssetPath scriptPath = LuaComponent_Info::luaFiles[i];
            char buf[512];
            if (scriptPath.m_assetRelativePath != L"")
                snprintf(buf, sizeof(buf), WStringToUtf8(scriptPath.m_assetRelativePath).c_str());
            else
                snprintf(buf, sizeof(buf), "Unknown");
            comboItems.push_back(eastl::string(buf));

            if (scriptPath == luaInfo->scriptPath) currentIndex = i + 1; // +1 because of None at 0
        }

        ImGui::Text("Lua script:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-FLT_MIN);

        const char* preview = comboItems.size() > 0 ? comboItems[currentIndex].c_str() : "None";
        if (ImGui::BeginCombo("##LuaCombo", preview)) {
            for (int i = 0; i < (int)comboItems.size(); ++i) {
                bool selected = (i == currentIndex);
                if (ImGui::Selectable(comboItems[i].c_str(), selected)) {
                    if (i == 0) {
                        luaInfo->scriptPath.m_assetRelativePath = L"";
                    }
                    else {
                        luaInfo->scriptPath = LuaComponent_Info::luaFiles[i - 1];
                    }
                    currentIndex = i;
                }
                if (selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();

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

void PropertyPanel::DrawAudioPanel()
{
    if (!m_AudioEditor) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Audio Editor is NULL");
        ImGui::TextDisabled("Check EditorApp initialization order");
        return;
    }
    static float listWidth = 200.0f;
    static std::string newTrackPath = "";
    static char newTrackName[256] = "";
    static float newVolume = 1.0f;
    static bool newLoop = false;
    static int newTagIndex = 0; // 0: sfx, 1: music, 2: ambient

    ImGui::BeginGroup();
    {
        ImGui::Text("Tracks Library");
        ImGui::Separator();
        if (ImGui::Button("Add Audio Track", ImVec2(listWidth, 30)))
        {
            // auto selectedPath = FileDialogManager::Get().OpenFile(
            //     FileDialogManager::DialogType::Audio, 
            //     L"Import Audio Track"
            // );
            //
            ImGui::OpenPopup("AddAudioPopup");
        }

        // AssetPath selectedPath = AssetPath(L"", AssetPath::AssetSource::Project);
        // Add new binding modal
        if (ImGui::BeginPopupModal("AddAudioPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

            AudioEditor::ScanAudioFiles();
            eastl::vector<eastl::string> comboItems;
            comboItems.reserve(AudioEditor::m_audioFiles.size() + 1);

            // First entry = None
            comboItems.push_back(eastl::string("None"));

            int currentIndex = 0; // default to None
            for (int i = 0; i < (int)AudioEditor::m_audioFiles.size(); ++i) {
                AssetPath audioPath = AudioEditor::m_audioFiles[i];
                char buf[512];
                if (audioPath.m_assetRelativePath != L"")
                    snprintf(buf, sizeof(buf), WStringToUtf8(audioPath.m_assetRelativePath).c_str());
                else
                    snprintf(buf, sizeof(buf), "Unknown");
                comboItems.push_back(eastl::string(buf));

                if (audioPath == m_selectedAudioPath) currentIndex = i + 1; // +1 because of None at 0
            }

            ImGui::Text("Audio script:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-FLT_MIN);

            const char* preview = comboItems.size() > 0 ? comboItems[currentIndex].c_str() : "None";
            if (ImGui::BeginCombo("##AudioCombo", preview)) {
                for (int i = 0; i < (int)comboItems.size(); ++i) {
                    bool selected = (i == currentIndex);
                    if (ImGui::Selectable(comboItems[i].c_str(), selected)) {
                        if (i == 0) {
                            m_selectedAudioPath.m_assetRelativePath = L"";
                            newTrackPath = "";
                        }
                        else {
                            m_selectedAudioPath = AudioEditor::m_audioFiles[i - 1];
                            newTrackPath = WStringToUtf8(m_selectedAudioPath.m_assetRelativePath).c_str();
                        }
                        currentIndex = i;
                    }
                    if (selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            if (ImGui::Button("Add##AddAudio", ImVec2(120, 0)))
            {
                if (m_selectedAudioPath.m_assetRelativePath != L"")
                {
                    std::filesystem::path fsPath(m_selectedAudioPath.GetFullPath().c_str());
                    std::string fileName = fsPath.stem().string();
                    strcpy(newTrackName, fileName.c_str());

                    ImGui::OpenPopup("Configure Audio Track");
                }
            }

            if (ImGui::BeginPopupModal("Configure Audio Track", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Add Audio Track to Library");
                ImGui::Separator();

                ImGui::Text("File: %s", newTrackPath.c_str());
                ImGui::Spacing();

                ImGui::Text("Track Settings:");

                ImGui::InputText("Track name", newTrackName, sizeof(newTrackName));
                ImGui::SameLine();
                ImGui::TextDisabled("(used in Lua scripts)");

                const char* tagItems[] = { "SFX", "Music", "Ambient" };
                ImGui::Combo("Category", &newTagIndex, tagItems, IM_ARRAYSIZE(tagItems));

                ImGui::SliderFloat("Volume", &newVolume, 0.0f, 2.0f, "%.2f");
                ImGui::Checkbox("Loop", &newLoop);

                ImGui::Spacing();
                ImGui::Separator();

                ImGui::BeginGroup();
                if (ImGui::Button("Add to Library", ImVec2(120, 0)))
                {
                    if (strlen(newTrackName) > 0 && !newTrackPath.empty())
                    {
                        AudioTrack newTrack;
                        newTrack.filePath = AssetPath(Utf8ToWString(newTrackPath.c_str()), AssetPath::AssetSource::Project);
                        newTrack.name = newTrackName;

                        switch (newTagIndex) {
                        case 0: newTrack.tag = "sfx"; break;
                        case 1: newTrack.tag = "music"; break;
                        case 2: newTrack.tag = "ambient"; break;
                        }

                        newTrack.loop = newLoop;
                        newTrack.volume = newVolume;

                        m_AudioEditor->AddTrack(newTrack);

                        newTrackPath = "";
                        memset(newTrackName, 0, sizeof(newTrackName));
                        newVolume = 1.0f;
                        newLoop = false;
                        newTagIndex = 0;

                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a Track ID");
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    newTrackPath = "";
                    memset(newTrackName, 0, sizeof(newTrackName));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndGroup();

                ImGui::EndPopup();

                // m_selectedAudioPath.m_assetRelativePath = L"";
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel##AddAudio", ImVec2(120, 0)))
            {
                m_selectedAudioPath.m_assetRelativePath = L"";
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // ImGui::SameLine();
        // if (ImGui::Button("Scan Assets", ImVec2(listWidth, 30))) {
        //     if (m_WorldEditor) {
        //         std::wstring assetsPath = EDITOR_ASSETS_DIR;
        //         m_AudioEditor->ScanAndSync(assetsPath);
        //     }
        // }

        ImGui::BeginChild("TrackList", ImVec2(listWidth, -40), true);

        const auto& trackList = m_AudioEditor->GetTrackList();
        if (!trackList.empty())
        {
            for (const auto& track : trackList)
            {
                std::string itemLabel = track.name + "##" + track.name;
                bool isSelected = (m_selectedAudioName == track.name);

                if (ImGui::Selectable(itemLabel.c_str(), isSelected))
                {
                    m_selectedAudioName = track.name;
                }
            }
        }
        else
        {
            ImGui::TextDisabled("No tracks found");
            ImGui::TextDisabled("Click 'Add Audio Track' to import");
        }

        ImGui::EndChild();

        // if (ImGui::Button("Import Audio", ImVec2(listWidth, 30)))
        // {
        //     std::filesystem::path selectedPath = FileDialogManager::Get().OpenFile(
        //         FileDialogManager::DialogType::Audio, 
        //         L"Import Audio Track"
        //     );
        //
        //     if (!selectedPath.empty())
        //     {
        //         std::filesystem::path destDir = ContentBrowserPanel::s_AssetsDirectory;
        //         std::filesystem::path destPath = destDir / selectedPath.filename();
        //
        //         if (std::filesystem::exists(destPath)) {
        //             try {
        //                 std::filesystem::copy_file(selectedPath, destPath);
        //                 const auto& trackList = m_AudioEditor->GetTrackList();
        //             } catch (std::filesystem::filesystem_error& e) {
        //                 LOG_EDITOR_ERROR("Import error %s", e.what());
        //             }
        //         }
        //     }
        // }
        // if (ImGui::Button("Scan Assets", ImVec2(listWidth, 30))) {
        //     m_AudioEditor->ScanForAudioFiles(ContentBrowserPanel::s_AssetsDirectory);
        // }
        // if (ImGui::Button("Add Track", ImVec2(listWidth, 30)))
        // {
        //     ImGui::OpenPopup("AddAudioPopup");
        // }
        //     
        // if (ImGui::BeginPopup("AddAudioPopup"))
        // {
        //     static char pathBuf[256] = "";
        //     ImGui::InputText("Path", pathBuf, 256);
            // if (ImGui::Button("Add"))
            // {
            //     m_AudioEditor->AddTrack(pathBuf);
            //     ImGui::CloseCurrentPopup();
            //     memset(pathBuf, 0, 256);
            // }
            //
            // ImGui::EndPopup();
        // }
    }
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    {
        ImGui::Text("Track Details");
        ImGui::Separator();
        AudioTrack* selectedTrack = m_AudioEditor->getTrack(m_selectedAudioName);
        ImGui::BeginChild("TrackDetails", ImVec2(0, -40), true);
        if (selectedTrack != nullptr)
        {
            EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header1);
            ImGui::Text("%s", selectedTrack->name.c_str());
            EditorUI::FontStyles::Pop();

            ImGui::Separator();

            ImGui::TextDisabled("File: %s", WStringToUtf8(selectedTrack->filePath.GetFullPath()).c_str());

            ImGui::Spacing();

            if (ImGui::Button("Play Preview", ImVec2(100, 0)))
            {
                m_AudioEditor->PlayPreview(selectedTrack->name);
            }

            ImGui::SameLine();

            if (ImGui::Button("Stop", ImVec2(100, 0)))
            {
                m_AudioEditor->StopPreview();
            }

            ImGui::SameLine();

            if (ImGui::Button("3D Test", ImVec2(80, 0)))
            {
                auto* audioSystem = m_AudioEditor->GetAudioSystem();
                if (audioSystem) {
                    audioSystem->Play3D(selectedTrack->name, 0, 0, 0, selectedTrack->volume);
                }
            }

            ImGui::Separator();

            ImGui::Text("Settings");

            static char editNameBuffer[256] = { 0 };
            static std::string editingTrackname = "";

            if (editingTrackname != selectedTrack->name) {
                strcpy(editNameBuffer, selectedTrack->name.c_str());
                editingTrackname = selectedTrack->name;
            }

            if (ImGui::InputText("Track Name", editNameBuffer, sizeof(editNameBuffer)))
            {
            }
            ImGui::SameLine();
            if (ImGui::Button("Update Name"))
            {
                std::string newName = editNameBuffer;
                if (!newName.empty() && newName != selectedTrack->name) {
                    m_AudioEditor->RenameTrack(selectedTrack->name, newName);
                    m_selectedAudioName = newName;
                }
            }

            int currentTag = 0;
            if (selectedTrack->tag == "music") currentTag = 1;
            else if (selectedTrack->tag == "ambient") currentTag = 2;

            if (ImGui::Combo("Category", &currentTag, "SFX\0Music\0Ambient\0"))
            {
                switch (currentTag) {
                case 0: selectedTrack->tag = "sfx"; break;
                case 1: selectedTrack->tag = "music"; break;
                case 2: selectedTrack->tag = "ambient"; break;
                }
            }

            float vol = selectedTrack->volume;
            if (ImGui::SliderFloat("Volume", &vol, 0.0f, 1.0f))
            {
                selectedTrack->volume = vol;
                m_AudioEditor->SetVolume(selectedTrack->name, vol);
            }

            if (ImGui::Checkbox("Loop", &selectedTrack->loop))
            {
                m_AudioEditor->SetLoop(selectedTrack->name, selectedTrack->loop);
                std::cout << "selectedTrack->loop = " << selectedTrack->loop << std::endl;
            }

            // static char tagBuf[64];
            // strncpy(tagBuf, selectedTrack->tag.c_str(), sizeof(tagBuf));
            // if (ImGui::InputText("Tag", tagBuf, sizeof(tagBuf)))
            // {
            //     selectedTrack->tag = std::string(tagBuf);
            // }

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("Delete Track", ImVec2(-1, 30)))
            {
                m_AudioEditor->RemoveTrack(selectedTrack->name);
                m_selectedAudioName = "";
            }
            ImGui::PopStyleColor();

            if (ImGui::Button("Save Audio Config", ImVec2(-1, 30)))
            {
                m_AudioEditor->SaveToJson();
            }
        }
        else
        {
            ImGui::TextDisabled("Select a track to edit details");
        }
        ImGui::EndChild();

    }
    ImGui::EndGroup();
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
        bool HasAllComponents = true;
        bool HasPhysicsComponent = obj->HasComponent<PhysicsComponent_Info>();
        bool HasCharacterComponent = obj->HasComponent<CharacterComponent_Info>();
        bool HasCharacterControllerComponent = obj->HasComponent<CharacterControllerComponent_Info>();

        ImGui::Text("Add Component");
        ImGui::Separator();

        if (!obj->HasComponent<MeshComponent_Info>())
        {
            HasAllComponents = false;

            if (ImGui::MenuItem("Mesh Component", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::MESH);
            }
        }

        if (!HasPhysicsComponent)
        {
            HasAllComponents = false;

            ImGui::BeginDisabled(HasCharacterControllerComponent);

            if (ImGui::MenuItem("Physics Component", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::PHYSICS);
            }

            ImGui::EndDisabled();

            if (HasCharacterControllerComponent)
                ImGui::SetItemTooltip("Can't have both Character Controller and Physics components");
        }

        if (!obj->HasComponent<TriggerComponent_Info>())
        {
            HasAllComponents = false;

            if (ImGui::MenuItem("Trigger Component", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::TRIGGER);
            }
        }

        if (!obj->HasComponent<PerceptionComponent_Info>())
        {
            HasAllComponents = false;

            if (ImGui::MenuItem("Perception Component", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::PERCEPTION);
            }
        }

        if (!obj->HasComponent<BehaviorController_Info>())
        {
            HasAllComponents = false;

            if (ImGui::MenuItem("Behavior Controller", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::BEHAVIOR);
            }
        }

        if (!obj->HasComponent<LuaComponent_Info>())
        {
            HasAllComponents = false;

            if (ImGui::MenuItem("Lua Script", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::LUA);
            }
        }

        if (!obj->HasComponent<CharacterComponent_Info>())
        {
            HasAllComponents = false;

            if (ImGui::MenuItem("Character Component", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::CHARACTER);
            }
        }

        if (!obj->HasComponent<CameraComponent_Info>())
        {
            HasAllComponents = false;

            if (ImGui::MenuItem("Camera Component", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::CAMERA);
                m_WorldEditor->m_scene->m_cameraManager->AddCamera(
                    obj->GetComponent<CameraComponent_Info>()->m_assignedComponent->m_camera);
            }
        }

        if (!obj->HasComponent<CharacterControllerComponent_Info>())
        {
            HasAllComponents = false;

            ImGui::BeginDisabled(HasPhysicsComponent || !HasCharacterComponent);

            if (ImGui::MenuItem("Character Controller", nullptr, false, true))
            {
                obj->AddDefaultComponent(SE::ComponentType::CHARACTER_CONTROLLER);
            }

            ImGui::EndDisabled();

            if (!HasCharacterComponent)
                ImGui::SetItemTooltip("Can't have Character Controller without Character component");
            else if (HasPhysicsComponent)
                ImGui::SetItemTooltip("Can't have both Character Controller and Physics components");
        }

        if (HasAllComponents)
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

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.4f, 0.4f, 0.4f, 0.6f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 0.8f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.3f, 0.3f, 0.3f, 0.6f });

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

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.4f, 0.4f, 0.4f, 0.6f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 0.8f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.3f, 0.3f, 0.3f, 0.6f });

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

void PropertyPanel::DrawMeshComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<MeshComponent_Info>())
        return;

    auto meshInfo = obj->GetComponent<MeshComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Mesh", flags))
    {
        EditorUI::FontStyles::Pop();

        if (!(obj->m_group == GameObjectGroup::Shapes))
        {
            if (DrawComponentRemoveButton<MeshComponent_Info>(obj))
            {
                ImGui::TreePop();

                auto tcInfo = obj->GetComponent<TransformComponent_Info>();
                tcInfo->m_assignedComponent->m_localPosition = DXSM::Vector3::Zero;
                tcInfo->m_assignedComponent->m_localRotation = DXSM::Vector3::Zero;
                tcInfo->m_assignedComponent->m_localScaleFactor = DXSM::Vector3::One;

                return;
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

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;


        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
        if (ImGui::TreeNodeEx("Mesh Transform", flags))
        {
            EditorUI::FontStyles::Pop();
            auto transform = obj->GetComponent<TransformComponent_Info>()->m_assignedComponent.get();

            ImguiUtils::DrawVector3Control("Mesh Offset", transform->m_localPosition,
                DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
                DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
                0.0f);

            DXSM::Vector3 localRotDeg = transform->m_localRotation * (180.0f / DirectX::XM_PI);
            if (ImguiUtils::DrawVector3Control("Mesh Rotation", localRotDeg,
                DXSM::Vector3(-360.0f, -360.0f, -360.0f),
                DXSM::Vector3(360.0f, 360.0f, 360.0f),
                0.0f))
            {
                transform->m_localRotation = localRotDeg * (DirectX::XM_PI / 180.0f);
            }

            ImguiUtils::DrawVector3Control("Mesh Scale", transform->m_localScaleFactor,
                DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
                DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
                1.0f);

            ImGui::TreePop();
        }
        else EditorUI::FontStyles::Pop();

        auto meshPtr = assigned->GetMesh();
        auto newMesh = DrawMeshSettings(meshPtr, obj->m_group, "Mesh");

        // Cull Mode
        auto cullMode = static_cast<int>(assigned->GetCullMode()) - 1;

        if (ImGui::Combo("Cull mode", &cullMode, "None\0Front\0Back\0"))
        {
            assigned->SetCullMode(static_cast<D3D11_CULL_MODE>(static_cast<int>(cullMode) + 1));
        }

        if (newMesh)
        {
            assigned->SetMesh(newMesh);
        }

        ImGui::Separator();
        // Texture
        auto tex = assigned->GetTexture();

        auto newTexture = DrawTextureSettings(tex, "Mesh");
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
        }
        else {
            ImGui::TextDisabled("Sampler: (none)");
        }
        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawCharacterComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<CharacterComponent_Info>())
        return;
    auto characterInfo = obj->GetComponent<CharacterComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Character Component", flags))
    {
        EditorUI::FontStyles::Pop();

        bool HasCharacterControllerComponent = obj->HasComponent<CharacterControllerComponent_Info>();
        ImGui::BeginDisabled(HasCharacterControllerComponent);

        if (DrawComponentRemoveButton<CharacterComponent_Info>(obj))
        {
            ImGui::TreePop();
            ImGui::EndDisabled();
            return;
        }

        if (HasCharacterControllerComponent)
            ImGui::SetItemTooltip("Can't delete Character Component before Character Controller");

        ImGui::EndDisabled();

        ImGui::Checkbox("Is player controlled", &characterInfo->m_assignedComponent->m_isPlayerControlled);

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawCharacterControllerComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<CharacterControllerComponent_Info>())
        return;
    auto charContrInfo = obj->GetComponent<CharacterControllerComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Character controller settings", flags))
    {
        EditorUI::FontStyles::Pop();

        if (DrawComponentRemoveButton<CharacterControllerComponent_Info>(obj))
        {
            ImGui::TreePop();
            return;
        }

        if (auto colliderData = charContrInfo->m_assignedComponent->m_colliderData)
        {
            DrawColliderSettings(colliderData, true);
        }

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
        if (ImGui::TreeNodeEx("Additional Forces", flags))
        {
            EditorUI::FontStyles::Pop();
            DrawFloatControl("Move speed", charContrInfo->m_assignedComponent->m_moveSpeed, 6.0f, 0.01f, 0.0f, 100.0f, "%.2f");
            DrawFloatControl("m_acceleration", charContrInfo->m_assignedComponent->m_acceleration, 30.0, 0.01f, 0.0f, 1000.0f, "%.2f");
            DrawFloatControl("Air acceleration", charContrInfo->m_assignedComponent->m_airAcceleration, 8.0f, 0.01f, 0.0f, 1000.0f, "%.2f");
            DrawFloatControl("Jump speed", charContrInfo->m_assignedComponent->m_jumpSpeed, 8.0f, 0.01f, 0.0f, 1000.0f, "%.2f");
            DrawFloatControl("m_gravity", charContrInfo->m_assignedComponent->m_gravity, -24.0f, 0.01f, -1000.0f, 1000.0f, "%.2f");
            DrawFloatControl("m_maxFallSpeed", charContrInfo->m_assignedComponent->m_maxFallSpeed, -24.0f, 0.01f, -1000.0f, 1000.0f, "%.2f");

            ImGui::Checkbox("m_enableStickToFloor", &charContrInfo->m_assignedComponent->m_enableStickToFloor);
            ImGui::Checkbox("m_enableWalkStairs", &charContrInfo->m_assignedComponent->m_enableWalkStairs);

            DrawFloatControl("m_stepHeight", charContrInfo->m_assignedComponent->m_stepHeight, 0.3f, 0.001f, 0.0f, 10.0f, "%.3f");
            DrawFloatControl("m_maxSlopeAngle", charContrInfo->m_assignedComponent->m_maxSlopeAngle, 45.0f, 0.01f, 0.0f, 90.0f, "%.2f");

            ImGui::TreePop();
        }
        else EditorUI::FontStyles::Pop();

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawCameraComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<CameraComponent_Info>())
        return;
    auto cameraInfo = obj->GetComponent<CameraComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Camera Component", flags))
    {
        EditorUI::FontStyles::Pop();

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
        ImGui::Text("View");
        EditorUI::FontStyles::Pop();

        cameraInfo->m_assignedComponent->m_camera->SetUpCameraViewByAspectRatio(640.0f / 360.0f);
        m_WorldEditor->m_miniViewRenderer->SetMainCamera(cameraInfo->m_assignedComponent->m_camera);
        m_WorldEditor->m_miniViewRenderer->Enable();
        ImVec2 avail = ImGui::GetContentRegionAvail();
        avail.y = avail.x * 360.0f / 640.0f;

        ImGui::Image((ImTextureID)m_WorldEditor->m_miniViewRenderer->m_GBuffer->pLightSRV.Get(), avail);

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
        ImGui::Text("Spring Arm Params");
        EditorUI::FontStyles::Pop();

        float stickLength = cameraInfo->m_assignedComponent->m_camera->m_springArmParams.length;
        if (ImGui::DragFloat("Length", &stickLength, 0.1f, 0.1f, 90.0f, "%.1f m"))
        {
            cameraInfo->m_assignedComponent->m_camera->m_springArmParams.length = stickLength;
        }

        DXSM::Vector3 springArmRotationDeg = cameraInfo->m_assignedComponent->m_camera->m_springArmParams.pitchYawRoll * (180.0f / DirectX::XM_PI);
        if (ImguiUtils::DrawVector3Control("Rotation", springArmRotationDeg,
            DXSM::Vector3(-90.0f, -80.0f, -360.0f),
            DXSM::Vector3(90.0f, 80.0f, 360.0f),
            0.0f))
        {
            cameraInfo->m_assignedComponent->m_camera->m_springArmParams.pitchYawRoll = springArmRotationDeg * (DirectX::XM_PI / 180.0f);
        }

        ImguiUtils::DrawVector3Control("Offset",
            cameraInfo->m_assignedComponent->m_camera->m_springArmParams.rootOffset,
            DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
            DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
            0.0f);

        EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
        ImGui::Text("Camera Params");
        EditorUI::FontStyles::Pop();

        DXSM::Vector3 cameraRotationDeg = cameraInfo->m_assignedComponent->m_camera->cameraPitchYawRoll * (180.0f / DirectX::XM_PI);
        if (ImguiUtils::DrawVector3Control("Rotation", cameraRotationDeg,
            DXSM::Vector3(-90.0f, -80.0f, -360.0f),
            DXSM::Vector3(90.0f, 80.0f, 360.0f),
            0.0f))
        {
            cameraInfo->m_assignedComponent->m_camera->cameraPitchYawRoll = cameraRotationDeg * (DirectX::XM_PI / 180.0f);
        }

        if (DrawComponentRemoveButton<CameraComponent_Info>(obj))
        {
            ImGui::TreePop();
            return;
        }

        ImGui::TreePop();
    }
    else
    {
        EditorUI::FontStyles::Pop();
    }
}

void PropertyPanel::DrawPerceptionComponent(GameObject_Info* obj)
{
    if (!obj->HasComponent<PerceptionComponent_Info>())
        return;

    auto percInfo = obj->GetComponent<PerceptionComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Perception Component Settings", flags))
    {
        EditorUI::FontStyles::Pop();

        if (DrawComponentRemoveButton<PerceptionComponent_Info>(obj))
        {
            ImGui::TreePop();
            return;
        }

        ImGui::Checkbox("Can See", &percInfo->CanSee);

        ImGui::Checkbox("Can See Through Objects", &percInfo->CanSeeThroughObjects);

        float SightRadius = percInfo->SightRadius;
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Sight Radius");
        ImGui::SameLine();
        if (ImGui::InputFloat("##SightRadius", &SightRadius))
        {
            percInfo->SetSightRadius(SightRadius);
        }

        float LoseRadius = percInfo->LoseRadius;
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Lose Radius");
        ImGui::SameLine();
        if (ImGui::InputFloat("##Lose Radius", &LoseRadius))
        {
            percInfo->SetLoseRadius(LoseRadius);
        }

        float FieldOfView = percInfo->FieldOfView;
        if (ImGui::InputFloat("Field Of View", &FieldOfView))
        {
            percInfo->SetFieldOfView(FieldOfView);
        }

        DXSM::Vector3 EyesOffset = percInfo->EyesOffset;
        if (ImguiUtils::DrawVector3Control("Eyes Offset", EyesOffset,
            DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
            DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
            0.0f));
        {
            percInfo->EyesOffset = EyesOffset;
        }

        ImGui::Separator();

        ImGui::Checkbox("Can Hear", &percInfo->CanHear);

        float HearingRadius = percInfo->HearingRadius;
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Hearing Radius");
        ImGui::SameLine();
        if (ImGui::InputFloat("##Hearing Radius", &HearingRadius))
        {
            percInfo->SetHearingRadius(HearingRadius);
        }

        float Threshold = percInfo->Threshold;
        if (ImGui::InputFloat("Threshold", &Threshold))
        {
            percInfo->SetThreshold(Threshold);
        }

        float Sensitivity = percInfo->Sensitivity;
        if (ImGui::InputFloat("Sensitivity", &Sensitivity))
        {
            percInfo->SetSensitivity(Sensitivity);
        }

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawBehaviorController(GameObject_Info* obj)
{
    if (!obj->HasComponent<BehaviorController_Info>())
        return;

    auto behaviorInfo = obj->GetComponent<BehaviorController_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Behavior Controller Settings", flags))
    {
        EditorUI::FontStyles::Pop();

        if (DrawComponentRemoveButton<BehaviorController_Info>(obj))
        {
            ImGui::TreePop();
            return;
        }

        ImGui::Checkbox("Is Enabled", &behaviorInfo->IsEnabled);

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

void PropertyPanel::DrawEmitterDetails(
    GameObject_Info* obj
    /*
    SE::ParticleData::EmitterPointConstantBuffer* emitterPointBuffer,
    SE::ParticleData::SimulateParticlesConstantBuffer* simulateParticlesBuffer
    */
)
{
    if (!obj)
        return;
    if (!obj->HasComponent<ParticleEmitterComponent_Info>())
        return;

    auto emitterInfo = obj->GetComponent<ParticleEmitterComponent_Info>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::Separator();
    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header2);
    if (ImGui::TreeNodeEx("Particle Emitter", flags))
    {
        EditorUI::FontStyles::Pop();

        auto emitterData = emitterInfo->m_particleData.get();
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


        ImguiUtils::DrawVector3Control("Position", emitterPointBuffer->position,
            DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
            DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
            0.0f);

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

        ImguiUtils::DrawVector3Control("Force vector", simulateParticlesBuffer->force,
            DXSM::Vector3(-1'000'000.0f, -1'000'000.0f, -1'000'000.0f),
            DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
            0.0f);

        ImGui::Separator();
        // Texture

        auto tex = emitterInfo->m_particleData->m_texture;

        auto newTexture = DrawTextureSettings(tex, "Emitter");
        if (newTexture)
        {
            newTexture->SetSlot(0u);
            emitterInfo->m_particleData->SetTexture(newTexture);
        }

        ImGui::Separator();
        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();
}

eastl::shared_ptr<SE_G::Bind::Texture> PropertyPanel::DrawTextureSettings(
    eastl::shared_ptr<SE_G::Bind::Texture> texture,
    eastl::string widgetGroup)
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
    if (ImGui::SmallButton(s_meshEditor.m_editTexture ?
        (eastl::string("Close Texture Editor##") + widgetGroup).c_str() : (eastl::string("Edit Texture##") + widgetGroup).c_str()))
    {
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
        ImGui::Combo(
            (eastl::string("Texture Source##") + widgetGroup).c_str(),
            (int*)&s_meshEditor.m_texAssetSource, srcItems);

        if (ImGui::Button(
            (eastl::string("Load Texture##") + widgetGroup).c_str()
        ))
        {
            s_meshEditor.m_texError.clear();

            AssetPath::AssetSource src = s_meshEditor.m_texAssetSource;
            std::string relNarrow = s_meshEditor.m_texPathBuf;
            std::wstring relWide(relNarrow.begin(), relNarrow.end());
            AssetPath ap(relWide.c_str(), src);

            // Using Resource manager
            ResourceHandle texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);

            if (texHandle.guid == 0) {
                // Error
                ap = AssetPath(
                    SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
                    AssetPath::AssetSource::Engine);
                texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
            }

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

            s_meshEditor.m_editTexture = false;
        }

        if (!s_meshEditor.m_texError.empty()) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Texture load error: %s", s_meshEditor.m_texError.c_str());
        }
    }
    return newTexture;
}

eastl::shared_ptr<SE_G::Mesh> PropertyPanel::DrawMeshSettings(
    eastl::shared_ptr<SE_G::Mesh> meshPtr, GameObjectGroup group,
    eastl::string widgetGroup)
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
        if (ImGui::SmallButton(s_meshEditor.m_editMesh ?
            (eastl::string("Close Mesh Editor##") + widgetGroup).c_str() : (eastl::string("Edit Mesh##") + widgetGroup).c_str()))
        {
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
            ImGui::Combo(
                (eastl::string("Mesh Source##") + widgetGroup).c_str(),
                (int*)&s_meshEditor.m_meshAssetSource, srcItems);

            if (ImGui::Button("Load Mesh")) {
                s_meshEditor.m_meshError.clear();

                AssetPath::AssetSource src = s_meshEditor.m_meshAssetSource;
                std::string relNarrow = s_meshEditor.m_meshPathBuf;
                std::wstring relWide(relNarrow.begin(), relNarrow.end());
                AssetPath ap(relWide.c_str(), src);

                /*
                {
                    newMesh = eastl::make_shared<SE_G::Mesh>(m_WorldEditor->m_renderer->GetDevice(), ap);
                    //assigned->SetMesh(newMesh);

                }
                */
                // Using Resource manager
                ResourceHandle meshHandle = ResourceManagerFacade::Instance().LoadByPath(ap);

                if (meshHandle.guid == 0) {
                    // Error
                    ap = AssetPath(L"Box_repeat");
                    meshHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
                }

                {
                    SE_G::Mesh* mesh =
                        ResourceManagerFacade::Instance().Get<SE_G::Mesh>(meshHandle);

                    if (mesh) {
                        newMesh = eastl::shared_ptr<SE_G::Mesh>(
                            mesh,
                            [](SE_G::Mesh*) {}
                        );
                        newMesh->m_meshPath = mesh->m_meshPath;
                    }
                    else {
                        s_meshEditor.m_texError = "Failed to cast loaded resource to Mesh";
                    }
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
        ImGui::Button((eastl::string("Edit Mesh##") + widgetGroup).c_str());
        ImGui::SetItemTooltip("Mesh editor disabled for shapes");
        ImGui::EndDisabled();
    }

    return newMesh;
}

void PropertyPanel::DrawColliderSettings(eastl::shared_ptr<SE::ColliderData> colliderData, bool fixedShapeType)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    EditorUI::FontStyles::Push(EditorUI::FontStyles::Style::Header3);
    if (ImGui::TreeNodeEx("Collider Settings", flags))
    {
        EditorUI::FontStyles::Pop();

        ImGui::BeginDisabled(fixedShapeType);
        auto currentShape = colliderData->GetShapeType();
        const char* shapeItems =
            "Sphere\0Box\0Capsule\0TaperedCapsule\0";
        if (ImGui::Combo("Shape Type", (int*)&currentShape, shapeItems))
        {
            colliderData->SetShapeType(currentShape);
        }
        ImGui::EndDisabled();

        auto settings = colliderData->GetColliderSettings();
        bool settingsChanged = false;

        switch (colliderData->GetShapeType())
        {
        case SE::ColliderShapeType::Box:
            if (ImguiUtils::DrawVector3Control("Box Size", settings.data.asBox.m_size,
                DXSM::Vector3(0.0001f, 0.0001f, 0.0001f),
                DXSM::Vector3(1'000'000.0f, 1'000'000.0f, 1'000'000.0f),
                1.0f))
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

        ImGui::TreePop();
    }
    else EditorUI::FontStyles::Pop();


    ImGui::Separator();
}