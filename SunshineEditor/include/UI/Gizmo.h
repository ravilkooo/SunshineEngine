#pragma once

#include <imgui.h>
#include <ImGuizmo.h>

#include <GameObject/GameObject.h>
#include <Graphics/Utils/Camera.h>
#include <Component/TransformComponent.h>
#include <UI/FontStyles.h>

class WorldEditor;

class Gizmo
{
public:
    Gizmo();
    ~Gizmo() = default;

    void Init();
    
    void Update();
    void Draw();
    
    void SetSelectedObject(GameObject_Info* obj);
    void SetWorldEditor(eastl::shared_ptr<WorldEditor> worldEditor);
    void SetViewportRect(const ImVec2& pos, const ImVec2& size);
    
    ImGuizmo::OPERATION GetCurrentOperation() const { return m_currentOperation; }
    ImGuizmo::MODE GetCurrentMode() const { return m_currentMode; }
    
    bool IsUsing() const { return ImGuizmo::IsUsing(); }
    bool IsOver() const { return ImGuizmo::IsOver(); }

private:
    void DrawGizmoControls();
    void ApplyTransform(float* matrix);
    void ExtractTransformFromMatrix(const float* matrix);
    void UpdateObjectMatrix();
    DXSM::Quaternion ConvertEulerToQuaternion(const DXSM::Vector3& euler) const;
    DXSM::Vector3 ConvertQuaternionToEuler(const DXSM::Quaternion& quat) const;

    eastl::shared_ptr<WorldEditor> m_worldEditor = nullptr;
    GameObject_Info* m_selectedObject = nullptr;
    
    ImGuizmo::OPERATION m_currentOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE m_currentMode = ImGuizmo::WORLD;
    
    bool m_useSnap = false;
    float m_snapValue[3] = { 1.f, 1.f, 1.f };
    float m_rotationSnap = 15.0f; 
    float m_scaleSnap = 0.1f;
    
    ImVec2 m_viewportPos = { 0, 0 };
    ImVec2 m_viewportSize = { 0, 0 };
    
    float m_objectMatrix[16] = { 0 };
    bool m_matrixInitialized = false;

    DXSM::Quaternion m_gizmoRotation;
    bool m_hasRotationCache = false;
};