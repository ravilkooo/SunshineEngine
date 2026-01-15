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
            FPS, ORBITAL, FOLLOW
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
        void Update(const DXSM::Matrix targetTransform);
        void Update(const DXSM::Matrix targetTransform, DXSM::Vector3 direction);
        void Update(const DXSM::Matrix targetTransform, DXSM::Vector3 direction, float referenceLen);

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

        void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget);
        void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget, DXSM::Vector3 spinAxis);
        void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget, DXSM::Vector3 spinAxis, float referenceLen);

        void SwitchProjection();

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

        DXSM::Vector3 position;
        DXSM::Vector3 followDirection;
        DXSM::Vector3 target;
        DXSM::Vector3 up;
        DXSM::Vector3 forward;
        DXSM::Vector3 right;

        DXSM::Matrix rotateCamToForward;

        struct FollowStickParams
        {
            float stickLength = 10.0f;

            float stickYaw = 0.0f;
            float stickPitch = 0.0f;

            DXSM::Vector3 viewPitchYawRoll = DXSM::Vector3::Zero;

            DXSM::Vector3 offset = DXSM::Vector3::Zero;
        } m_stickParams;
        DXSM::Vector3 stickDirection;
        
        DXSM::Vector3 GetStickDirection();
        float GetStickLength() { return m_stickParams.stickLength; };
        void SetStickLength(float newLen) { m_stickParams.stickLength = fmin(fmax(0.0f, newLen), 100.0f); };

        void RotateStickYawPitch(float yawSpeed, float pitchSpeed);

        float m_deltaTime = 1.0f;
    private:
        void SetFOV(float fov);
        void SetAspectRatio(float aspectRatio);
        void SetViewWidth(float viewWidth);
        void SetViewHeight(float viewHeight);

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


        CAMERA_MODE cameraMode = CAMERA_MODE::FPS;

        // for ORBITAL camera mode
        DXSM::Vector3 orbitalTarget;
        float minOrbitalDistance;
        float orbitalDistance;
        float orbitalYaw;
        float orbitalPitch;
        float orbitalAngleSpeed;
        //float orbitalAngleSpeed;
        DXSM::Vector3 spinAxis;

        // for FOLLOW camera mode
        float followPitch;

        // Follow PlayerObject
        bool m_playerAsObject = false;

        union {
            Scene* asScene;
            Scene_Info* asInfo;
        } m_scene;

        /*
        union {
            PlayerObject* asObject;
            PlayerObject_Info* asInfo;
        } m_player;
        */
        TransformComponent* m_playerTransform = nullptr;
        bool m_playerPointerInited = false;

        SE::UUID m_playerUUID = SE::UUID(0u);

    };
}
