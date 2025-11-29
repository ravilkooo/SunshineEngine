#pragma once

#include <EASTL/shared_ptr.h>
#include "SimpleMath.h"
#include <Graphics/Lighting/LightData.h>
#include <GameObject/Shapes/ShapeCollection.h>


#include "GameObject/Shapes/ShapeObject.h"
#include "Utils/UUID.h"

class GameObject_Info;
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
    
    void SetSelectedUUID(SE::UUID uuid) { 
        m_SelectedUUID = uuid; 
    }
    
    void OnImGuiRender();

private:
    eastl::shared_ptr<WorldEditor> m_WorldEditor;
    SE::UUID m_SelectedUUID = SE::UUID(0u);
    
    void DrawGameObjectHeader(GameObject_Info* obj);

    void DrawTransformComponent(GameObject_Info* obj);
    void DrawDetails(GameObject_Info* obj);
    void DrawComponentAddPopup(GameObject_Info* obj);

    void DrawAmbientLightDetails(SE_G::AmbientLightData* lightData);
    void DrawDirectionalLightDetails(SE_G::DirectionalLightData* lightData);
    void DrawPointLightDetails(SE_G::PointLightData* lightData);
    
    void DrawSkyBoxDetails(SE_G::SkyBoxData* lightData);

    void DrawBoxShapeDetails(BoxShapeObject_Info* obj);
    void DrawSphereShapeDetails(SphereShapeObject_Info* obj);
    void DrawGeosphereShapeDetails(GeosphereShapeObject_Info* obj);

    void DrawPhysicsComponent(GameObject_Info* obj);

    void DrawLuaComponent(GameObject_Info* obj);
    void DrawLuaFunctions(LuaComponent* luaComp);
    
    bool DrawVector3Control(const char* label, DirectX::SimpleMath::Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
    bool DrawFloatControl(const char* label, float& value, float resetValue = 0.0f,
                       float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                       const char* format = "%.3f", float columnWidth = 100.0f);

    bool DrawUIntControl(const char* label, uint32_t& value, uint32_t resetValue = 0,
                       float speed = 1.0f, uint32_t min = 0, uint32_t max = 100,
                       const char* format = "%u", float columnWidth = 100.0f);
};