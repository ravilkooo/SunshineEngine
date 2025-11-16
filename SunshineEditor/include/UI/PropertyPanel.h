#pragma once

#include <EASTL/shared_ptr.h>
#include "SimpleMath.h"
#include "Graphics/Renderer/Technique/LightTechnique.h"


#include "Utils/UUID.h"

class GameObject;
class TransformComponent;
class RenderComponent;
class LuaComponent;
class WorldEditor;

class PropertyPanel
{
public:
    PropertyPanel();
    
    void SetWorldEditor(eastl::shared_ptr<WorldEditor> worldEditor) { 
        m_WorldEditor = worldEditor; 
    }
    
    void SetSelectedUUID(Sunshine::UUID uuid) { 
        m_SelectedUUID = uuid; 
    }
    
    void OnImGuiRender();

private:
    eastl::shared_ptr<WorldEditor> m_WorldEditor;
    Sunshine::UUID m_SelectedUUID = Sunshine::UUID(0u);
    
    void DrawGameObjectHeader(GameObject* obj);

<<<<<<< Updated upstream
    void DrawTransformComponent(GameObject* obj);
    void DrawRenderComponent(GameObject* obj);
    void DrawComponentAddPopup(GameObject* obj);
=======
    void DrawTransformComponent(GameObject_Info* obj);
    void DrawDetails(GameObject_Info* obj);
    void DrawComponentAddPopup(GameObject_Info* obj);
>>>>>>> Stashed changes
    
    
    void DrawAmbientLightDetails(SE_G::AmbientLightData* lightData);
    void DrawDirectionalLightDetails(SE_G::DirectionalLightData* lightData);
    void DrawPointLightDetails(SE_G::PointLightData* lightData);
    void DrawSkyBoxDetails(SE_G::SkyBoxData* lightData);
    
    void DrawLuaComponent(GameObject* obj);
    void DrawLuaFunctions(LuaComponent* luaComp);

    bool DrawVector3Control(const char* label, DirectX::SimpleMath::Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
};