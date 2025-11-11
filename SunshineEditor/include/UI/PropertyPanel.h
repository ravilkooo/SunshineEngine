#pragma once

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>
#include <imgui.h>
#include "SimpleMath.h"


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
    void DrawRenderComponent(GameObject* obj);
    void DrawLuaComponent(GameObject* obj);
    void DrawComponentAddPopup(GameObject* obj);
    
    void DrawLuaFunctions(LuaComponent* luaComp);
    

    bool DrawVector3Control(const char* label, DirectX::SimpleMath::Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
};