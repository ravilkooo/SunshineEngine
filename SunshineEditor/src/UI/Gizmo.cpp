#include "UI/Gizmo.h"

#include <Graphics/Renderer/DeferredRenderer.h>

#include <Component/TransformComponent.h>

#include "WorldEditor.h"

Gizmo::Gizmo()
{
    memset(m_objectMatrix, 0, sizeof(float) * 16);
    m_objectMatrix[0] = m_objectMatrix[5] = m_objectMatrix[10] = m_objectMatrix[15] = 1.0f;
    m_currentMode = ImGuizmo::WORLD;

    m_snapValue[0] = 0.5f;  
    m_snapValue[1] = 0.5f;    
    m_snapValue[2] = 0.5f;  
    m_rotationSnap = 15.0f;  
    m_scaleSnap = 0.1f;
    
    m_useSnap = false;
    m_hasRotationCache = false;
    
}

void Gizmo::Init()
{
    // ImGuizmo::SetOrthographic(false);
    ImGuizmo::Enable(true);
    ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
    // ImGuizmo::AllowAxisFlip(false);
}

void Gizmo::Update()
{
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
        return;
    
    if (ImGui::IsKeyPressed(ImGuiKey_W))
    {
        m_currentOperation = ImGuizmo::TRANSLATE;
        m_currentMode = ImGuizmo::WORLD; 
    }
    if (ImGui::IsKeyPressed(ImGuiKey_E))
    {
        m_currentOperation = ImGuizmo::ROTATE;
        m_currentMode = ImGuizmo::LOCAL; 
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_R))
    {
        m_currentOperation = ImGuizmo::SCALE;
        m_currentMode = ImGuizmo::LOCAL;
    }
    // if (ImGui::IsKeyPressed(ImGuiKey_T))
    //     m_currentMode = (m_currentMode == ImGuizmo::WORLD) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
    
}

void Gizmo::Draw()
{
    if (!m_selectedObject || m_viewportSize.x <= 0 || m_viewportSize.y <= 0)
        return;

    auto transformComp = m_selectedObject->GetComponent<TransformComponent_Info>();
    if (!transformComp || !transformComp->m_assignedComponent)
        return;

    if (!ImGuizmo::IsUsing())
    {
        UpdateObjectMatrix();
    }

    auto camera = m_worldEditor->m_renderer->GetMainCamera();
    if (!camera)
        return;
    
    DXSM::Matrix viewMatrix = camera->GetViewMatrix();
    DXSM::Matrix projectionMatrix = camera->GetProjectionMatrix();
    
    DXSM::Matrix flipZ;
    flipZ = DXSM::Matrix::CreateScale(1, 1, -1); 
    
    viewMatrix = viewMatrix * flipZ;
    
    projectionMatrix.m[2][0] *= -1; 
    projectionMatrix.m[2][1] *= -1;
    projectionMatrix.m[2][2] *= -1;
    projectionMatrix.m[2][3] *= -1;
    
    float view[16], projection[16];
    memcpy(view, &viewMatrix, sizeof(float) * 16);
    memcpy(projection, &projectionMatrix, sizeof(float) * 16);

    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(m_viewportPos.x, m_viewportPos.y, m_viewportSize.x, m_viewportSize.y);
    
    DrawGizmoControls();
    
    float* snapValues = nullptr;
    if (m_useSnap)
    {
        switch (m_currentOperation)
        {
        case ImGuizmo::TRANSLATE:
            snapValues = m_snapValue;
            break;
        case ImGuizmo::ROTATE:
            snapValues = &m_rotationSnap;
            break;
        case ImGuizmo::SCALE:
            snapValues = &m_scaleSnap;
            break;
        }
    }
    
    ImGuizmo::SetGizmoSizeClipSpace(0.18f);

    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 20.0f);
    
    bool changed = ImGuizmo::Manipulate(
                        view,
                        projection,
                        m_currentOperation,
                        m_currentMode,
                        m_objectMatrix,
                        nullptr,
                        snapValues
                    );
    
    ImGui::PopStyleVar();
    
    if (changed)
    {
        ApplyTransform(m_objectMatrix);
    }
}

void Gizmo::UpdateObjectMatrix()
{
    auto transformInfo = m_selectedObject
        ->GetComponent<TransformComponent_Info>();
    if (!transformInfo || !transformInfo->m_assignedComponent)
        return;

    auto& comp = *transformInfo->m_assignedComponent;

    if (!m_hasRotationCache)
    {
        m_gizmoRotation = ConvertEulerToQuaternion(comp.m_rotation);
        m_hasRotationCache = true;
    }

    DXSM::Matrix worldMatrix =
        DXSM::Matrix::CreateScale(comp.m_scaleFactor) *
        DXSM::Matrix::CreateFromQuaternion(m_gizmoRotation) *
        DXSM::Matrix::CreateTranslation(comp.m_position);

    memcpy(m_objectMatrix, &worldMatrix, sizeof(float) * 16);

}

void Gizmo::DrawGizmoControls()
{   
    float padding = 5.0f;
    
    ImVec2 windowPos = ImVec2(
        m_viewportPos.x + m_viewportSize.x, 
        m_viewportPos.y            
    );
    
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding - 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(padding * 0.5f, padding * 0.5f));
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

    ImGui::Begin("Gizmo Controls", nullptr, window_flags);

    static bool showAdvancedSettings = false;
    
    ImGuiDir arrowDir = showAdvancedSettings ? ImGuiDir_Down : ImGuiDir_Right;
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.4f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

    if (ImGui::ArrowButton("##GizmoToggle", arrowDir))
    {
        showAdvancedSettings = !showAdvancedSettings;
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    
    if (ImGui::RadioButton("Translate (W)", m_currentOperation == ImGuizmo::TRANSLATE))
        m_currentOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate (E)", m_currentOperation == ImGuizmo::ROTATE))
        m_currentOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale (R)", m_currentOperation == ImGuizmo::SCALE))
        m_currentOperation = ImGuizmo::SCALE;

    if (showAdvancedSettings)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // if (m_currentOperation == ImGuizmo::SCALE)
        // {
        //     ImGui::Spacing();
        //     if (ImGui::RadioButton("World (T)", m_currentMode == ImGuizmo::WORLD))
        //         m_currentMode = ImGuizmo::WORLD;
        //     ImGui::SameLine();
        //     if (ImGui::RadioButton("Local (T)", m_currentMode == ImGuizmo::LOCAL))
        //         m_currentMode = ImGuizmo::LOCAL;
        // }
        //
        // ImGui::Spacing();
        ImGui::Checkbox("Use Snap", &m_useSnap);
        if (m_useSnap)
        {
            switch (m_currentOperation)
            {
            case ImGuizmo::TRANSLATE:
                ImGui::InputFloat3("Translation Snap", m_snapValue);
                break;
            case ImGuizmo::ROTATE:
                ImGui::InputFloat("Rotation Snap", &m_rotationSnap);
                break;
            case ImGuizmo::SCALE:
                ImGui::InputFloat("Scale Snap", &m_snapValue[0]);
                break;
            }
        }
    }
    
    ImGui::End();
       
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

DXSM::Quaternion Gizmo::ConvertEulerToQuaternion(const DXSM::Vector3& euler) const
{
    return DXSM::Quaternion::CreateFromRotationMatrix(
        DXSM::Matrix::CreateRotationY(DX::XMConvertToRadians(euler.y)) *
        DXSM::Matrix::CreateRotationX(DX::XMConvertToRadians(euler.x)) *
        DXSM::Matrix::CreateRotationZ(DX::XMConvertToRadians(euler.z))
    );
}

DXSM::Vector3 Gizmo::ConvertQuaternionToEuler(const DXSM::Quaternion& quat) const
{
    
    float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
    
    float sinp = 2.0f * (w * x - y * z);
    if (fabsf(sinp) >= 1.0f)
    {
        float pitch = copysignf(DX::XM_PIDIV2, sinp);  
        float yaw = atan2f(2.0f * (w * y + x * z), 1.0f - 2.0f * (x * x + y * y));
        float roll = 0.0f;
        return DXSM::Vector3(pitch, yaw, roll);
    }
    else
    {
        float pitch = asinf(sinp);
        float yaw = atan2f(2.0f * (w * y + x * z), 1.0f - 2.0f * (x * x + y * y));
        float roll = atan2f(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
        return DXSM::Vector3(pitch, yaw, roll);
    }
}


void Gizmo::ApplyTransform(float* matrix)
{
    if (!m_selectedObject)
        return;
    
    ExtractTransformFromMatrix(matrix);
}

void Gizmo::ExtractTransformFromMatrix(const float* matrix)
{
    auto transformInfo = m_selectedObject
        ->GetComponent<TransformComponent_Info>();

    if (!transformInfo || !transformInfo->m_assignedComponent)
        return;

    auto& comp = *transformInfo->m_assignedComponent;

    DXSM::Matrix m;
    memcpy(&m, matrix, sizeof(float) * 16);
    
    DX::XMVECTOR scale, rotation, translation;
    DX::XMMatrixDecompose(&scale, &rotation, &translation, DX::XMLoadFloat4x4(&m));

    DX::XMStoreFloat3(&comp.m_position, translation);
    DX::XMStoreFloat3(&comp.m_scaleFactor, scale);

    if (m_currentOperation != ImGuizmo::SCALE)
    {
        DX::XMStoreFloat4(&m_gizmoRotation, rotation);
        m_hasRotationCache = true;
        
        comp.m_rotation = ConvertQuaternionToEuler(m_gizmoRotation);
    }
    else
    {
        m_hasRotationCache = false;
    }
}

void Gizmo::SetSelectedObject(GameObject_Info* obj)
{
    if (m_selectedObject == obj)
        return; 

    m_selectedObject = obj;
    m_hasRotationCache = false;

    if (obj)
        UpdateObjectMatrix();
}

void Gizmo::SetWorldEditor(eastl::shared_ptr<WorldEditor> worldEditor)
{
    m_worldEditor = worldEditor;
}

void Gizmo::SetViewportRect(const ImVec2& pos, const ImVec2& size)
{
    m_viewportPos = pos;
    m_viewportSize = size;
}