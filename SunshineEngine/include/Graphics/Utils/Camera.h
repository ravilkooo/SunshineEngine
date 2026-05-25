#pragma once

#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>

#include <DirectXMath.h>
#include <SimpleMath.h>

#include "Graphics/Bindable/ConstantBuffer.h"
#include <Utils/UUID.h>

namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;

class Scene;
class Scene_Info;
class TransformComponent;

namespace SE_G {
    class Camera
    {
        friend class ShadowMapPass;
    public:
        struct CameraBuffer {
            DXSM::Matrix viewProjMatrix;
            DXSM::Vector3 camPos;
            float pad;
        };

        eastl::unique_ptr<Bind::VertexConstantBuffer<CameraBuffer>> cameraBuffer;

        enum class CAMERA_MODE
        {
            FPS, FOLLOW
        };

        Camera(ID3D11Device* device);
        Camera(ID3D11Device* device, float aspectRatio);
        ~Camera();

        void InitBuffer(ID3D11Device* device);
        void UpdateBuffer(ID3D11DeviceContext* context);
        void BindBuffer(ID3D11DeviceContext* context);

        void SetPosition(DXSM::Vector3 position);
        DXSM::Vector3 GetPosition();

        void SetTarget(DXSM::Vector3 target);
        DXSM::Vector3 GetTarget();

        void SetUp(DXSM::Vector3 up);
        DXSM::Vector3 GetUp();

        void SetNearZ(float nearZ);
        float GetNearZ();

        void SetFarZ(float farZ);
        float GetFarZ();

        void SetUpCameraViewByAspectRatio(float newAspectRatio);
        void SetUpCameraViewByAspectRatio_horizontal(float newAspectRatio);
        void SetUpCameraViewByAspectRatio_vertical(float newAspectRatio);
        void SetUpCameraViewByFOV(float newFOV);
        /*
        void SetUpCameraViewByFOV_horizontal(float newFOV);
        void SetUpCameraViewByFOV_vertical(float newFOV);
        */

        void ResetCameraView(float newAspectRatio);

        float GetViewWidth();
        float GetViewHeight();

        void SetReferenceLen(float referenceLen);
        float GetReferenceLen();

        void Update(float deltaTime);
        void Update(const DXSM::Vector3 targetPoistion);

        DX::XMMATRIX GetViewMatrix();
        DX::XMMATRIX GetProjectionMatrix() const;

        void MoveForward(float speed);
        void MoveBackward(float speed);
        void MoveLeft(float speed);
        void MoveRight(float speed);
        void MoveUp(float speed);
        void MoveDown(float speed);

        void RotateYaw(float angle);
        void RotatePitch(float angle);

        void SwitchToFPSMode();

        void AssignScene(Scene* scene);
        void AssignScene(Scene_Info* scene);

        void SetFollowPlayer(SE::UUID playerUUID);
        void InitFollowModeParams();

        /*
        void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget);
        void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget, DXSM::Vector3 spinAxis);
        void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget, DXSM::Vector3 spinAxis, float referenceLen);
        */

        void SwitchProjection();
        bool IsPerspectiveCamera();

        struct FrustumPlanes {
            DX::XMVECTOR Left;
            DX::XMVECTOR Right;
            DX::XMVECTOR Top;
            DX::XMVECTOR Bottom;
            DX::XMVECTOR Near;
            DX::XMVECTOR Far;
        };

        FrustumPlanes GetFrustumPlanes();

        struct FrustumCorners {
            DX::XMVECTOR Near[4];
            DX::XMVECTOR Far[4];
        };

        FrustumCorners GetFrustumCorners();


        // view options
        // spring arm options
        struct FollowSpringArmParams
        {
            float length = 10.0f;
            DXSM::Vector3 pitchYawRoll = DXSM::Vector3::Zero;
            DXSM::Vector3 rootOffset = DXSM::Vector3::Zero;
        } m_stickParams;
        // camera options
        DXSM::Vector3 cameraOffset = DXSM::Vector3::Zero;
        DXSM::Vector3 cameraRotation = DXSM::Vector3::Zero;

        // auxiliary values (based on view params)
        DXSM::Vector3 position;
        DXSM::Vector3 followDirection;
        DXSM::Vector3 target;
        DXSM::Vector3 up;
        DXSM::Vector3 forward;
        DXSM::Vector3 right;

        DXSM::Vector3 GetStickRotation();
        void SetStickRotation(DXSM::Vector3 newRotation);

        float GetStickLength() { return m_stickParams.length; };
        void SetStickLength(float newLen) { m_stickParams.length = fmin(fmax(0.0f, newLen), 1000.0f); };

        void RotateStickYawPitch(float yawSpeed, float pitchSpeed);
        void RollStick(float rollSpeed);

        float m_deltaTime = 1.0f;
    private:
        void SetFOV(float fov);
        void SetAspectRatio(float aspectRatio);
        void SetViewWidth(float viewWidth);
        void SetViewHeight(float viewHeight);

        // projection options
        bool isPerspective = true;
        float fov;
        float aspectRatio;
        float nearZ;
        float farZ;
        float orthZ;
        float referenceLen;

        // for Orthographic projection
        float viewWidth;
        float viewHeight;

        // camera mode
        CAMERA_MODE cameraMode = CAMERA_MODE::FPS;

        // for FOLLOW camera mode
        float followPitch;

        // Follow PlayerObject
        bool m_playerAsObject = false;

        union {
            Scene* asScene;
            Scene_Info* asInfo;
        } m_scene;

        TransformComponent* m_playerTransform = nullptr;
        bool m_playerPointerInited = false;

        SE::UUID m_playerUUID = SE::UUID(0u);

    };
}
