#pragma once
#include <functional>

#include <EASTL/shared_ptr.h>
#include <SimpleMath.h>

#include <GameObject/GameObject.h>
#include <Graphics/Lighting/LightData.h>

#include <Utils/UUID.h>
#include <Utils/AssetPath.h>

class GameObject_Info;
class BoxShapeObject_Info;
class SphereShapeObject_Info;
class GeosphereShapeObject_Info;
class CylinderShapeObject_Info;
class SkyBox_Info;

class TransformComponent;
class RenderComponent;
class LuaComponent;
class WorldEditor;
class AudioEditor;

namespace SE {
    class ColliderData;
}

namespace SE_G {
    namespace Bind {
        class Texture;
    }
    class Mesh;
}

class PropertyPanel
{
public:
    static struct MeshEditor
    {
        bool m_editMesh;

        char m_meshPathBuf[512];
        AssetPath::AssetSource m_meshAssetSource;
        eastl::string m_meshError;

        bool m_editTexture;

        char m_texPathBuf[512];
        AssetPath::AssetSource m_texAssetSource;
        eastl::string m_texError;
    } s_meshEditor;

    PropertyPanel();
    
    void SetWorldEditor(eastl::shared_ptr<WorldEditor> worldEditor) { 
        m_WorldEditor = worldEditor; 
    }
    
    void SetSelectedUUID(SE::UUID uuid) { 
        m_SelectedUUID = uuid; 
    }

    void SetAudioEditor(AudioEditor* audioEditor) { 
        m_AudioEditor = audioEditor; 
    }
    
    void OnImGuiRender();

    void DrawAudioPanel();
private:
    eastl::shared_ptr<WorldEditor> m_WorldEditor;
    SE::UUID m_SelectedUUID = SE::UUID(0u);
    AudioEditor* m_AudioEditor = nullptr;
    AssetPath m_selectedAudioPath = AssetPath(L"", AssetPath::AssetSource::Project);
    
    std::string m_selectedAudioName = "";
    // AssetPath m_selectedAudioName = AssetPath(L"", AssetPath::AssetSource::Project);
    
    bool DrawGameObjectHeader(GameObject_Info* obj);

    void DrawParentnes(GameObject_Info* obj);

    void DrawGraphicsSettings(GameObject_Info* obj);
    void DrawTransformComponent(GameObject_Info* obj);
    void DrawDetails(GameObject_Info* obj);
    void DrawComponentAddPopup(GameObject_Info* obj);

    void DrawEmitterDetails(
        GameObject_Info* obj
        /*
        SE::ParticleData::EmitterPointConstantBuffer* emitterPointBuffer,
        SE::ParticleData::SimulateParticlesConstantBuffer* simulateParticlesBuffer
        */
        );

    void DrawAmbientLightDetails(SE_G::AmbientLightData* lightData);
    void DrawDirectionalLightDetails(SE_G::DirectionalLightData* lightData);
    void DrawPointLightDetails(SE_G::PointLightData* lightData);
    void DrawSpotLightDetails(SE_G::SpotLightData* lightData);
    
    void DrawSkyBoxDetails(SkyBox_Info* skyBoxObj);

    void DrawBoxShapeDetails(BoxShapeObject_Info* obj);
    void DrawSphereShapeDetails(SphereShapeObject_Info* obj);
    void DrawGeosphereShapeDetails(GeosphereShapeObject_Info* obj);
    void DrawCylinderShapeDetails(CylinderShapeObject_Info* obj);

    void DrawPhysicsComponent(GameObject_Info* obj);
    void DrawTriggerComponent(GameObject_Info* obj);
    void DrawColliderSettings(eastl::shared_ptr<SE::ColliderData> colliderData, bool fixedShapeType = false);

    void DrawMeshComponent(GameObject_Info* obj);

    void DrawCharacterComponent(GameObject_Info* obj);
    void DrawCharacterControllerComponent(GameObject_Info* obj);
    void DrawGrabComponent(GameObject_Info* obj);
    void DrawCameraComponent(GameObject_Info* obj);
    void DrawBouncePadComponent(GameObject_Info* obj);
    void DrawMovingPlatformComponent(GameObject_Info* obj);

    void DrawPerceptionComponent(GameObject_Info* obj);
    void DrawBehaviorController(GameObject_Info* obj);

    void DrawLuaComponent(GameObject_Info* obj);
    
    bool DrawFloatControl(const char* label, float& value, float resetValue = 0.0f,
                       float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                       const char* format = "%.3f", float columnWidth = 100.0f);

    bool DrawUIntControl(const char* label, uint32_t& value, uint32_t resetValue = 0,
                       float speed = 1.0f, uint32_t min = 0, uint32_t max = 100,
                       const char* format = "%u", float columnWidth = 100.0f);

    eastl::shared_ptr<SE_G::Bind::Texture> DrawTextureSettings(
        eastl::shared_ptr<SE_G::Bind::Texture> texture,
        eastl::string widgetGroup);

    eastl::shared_ptr<SE_G::Mesh> DrawMeshSettings(
        eastl::shared_ptr<SE_G::Mesh> meshPtr, GameObjectGroup group,
        eastl::string widgetGroup);

	template <typename T>
    bool DrawComponentRemoveButton(GameObject_Info* obj)
    {
        // Button width
        const char* labelRemove = "Remove component";
        ImVec2 textSize = ImGui::CalcTextSize(labelRemove);
        ImVec2 padding = ImGui::GetStyle().FramePadding;
        float labelWidth = textSize.x + padding.x * 2.0f;

        // free space on this line
        ImVec2 avail = ImGui::GetContentRegionAvail();
        avail.x = avail.x - textSize.x;

        if (avail.x > labelWidth) {
            // put on the same line
            float oldX = ImGui::GetCursorPosX();
            // ImGui::SameLine();
            ImGui::SetCursorPosX(oldX + avail.x);
        }
        // else dont call SameLine, Button will be under line

        if (ImGui::SmallButton(labelRemove)) {
            obj->RemoveComponent<T>();
            return true;
        }
        return false;
    }
};