#pragma once

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>
#include <imgui.h>
#include "SimpleMath.h"
#include "Graphics/Renderer/Technique/GPassTechnique.h"
#include "Graphics/Renderer/Technique/IconTechnique.h"
#include "Graphics/Renderer/Technique/LightTechnique.h"
#include "Graphics/Renderer/Technique/RenderTechnique.h"


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
    void DrawTransformComponent(GameObject* obj);
    void DrawAmbientLightTechniqueDetails(SE_G::LightTechnique<SE_G::AmbientLightData>* light_technique);
    void DrawDirectionalLightTechniqueDetails(SE_G::LightTechnique<SE_G::DirectionalLightData>* light_technique);
    void DrawIconTechniqueDetails(SE_G::IconTechnique* icon_technique);
    void DrawPointLightTechniqueDetails(SE_G::LightTechnique<SE_G::PointLightData>* light_technique);
    void DrawSkyBoxTechniqueDetails(SE_G::LightTechnique<SE_G::SkyBoxData>* light_technique);
    void DrawGPassTechniqueDetails(SE_G::GPassTechnique* pass_technique);
    void DrawTechniqueDetails(SE_G::RenderTechnique* get, const eastl::string& string);
    void DrawCommonTechniqueInfo(SE_G::RenderTechnique* tech);
    void DrawRenderComponent(GameObject* obj);
    void DrawLuaComponent(GameObject* obj);
    void DrawComponentAddPopup(GameObject* obj);
    
    void DrawLuaFunctions(LuaComponent* luaComp);
    

    bool DrawVector3Control(const char* label, DirectX::SimpleMath::Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
};