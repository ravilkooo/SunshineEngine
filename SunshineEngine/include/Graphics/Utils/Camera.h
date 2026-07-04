#pragma once

#include <d3d11.h>

#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>

#include <Utils/UUID.h>

#include <DirectXMath.h>
#include <SimpleMath.h>
namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;

// class Scene;
// class Scene_Info;
class TransformComponent;

namespace SE_G {
    namespace Bind {
        template <typename T>
        class VertexConstantBuffer;
    }

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

        SE::UUID GetAssignedUUID();

        void InitBuffer(ID3D11Device* device);
        void UpdateBuffer(ID3D11DeviceContext* context);
        void BindBuffer(ID3D11DeviceContext* context);

        void SetPosition(DXSM::Vector3 position);
        DXSM::Vector3 GetPosition();

        void SetTarget(DXSM::Vector3 target);
        DXSM::Vector3 GetTarget();

        void SetUp(DXSM::Vector3 up);
        DXSM::Vector3 GetUp();
        DXSM::Vector3 GetRight();
        DXSM::Vector3 GetForward();

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
        void UpdateTargetPoistion(const DXSM::Vector3 targetPoistion);

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

        void SetFollowUUID(SE::UUID followUUID);
        void AssignTransformComponent(TransformComponent* trComp);
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
        } m_springArmParams;
        float m_zoomAcceleration = 1.0f;
        // camera options
        DXSM::Vector3 cameraPitchYawRoll = DXSM::Vector3::Zero;

        // auxiliary values (based on view params)
        DXSM::Vector3 position;
        DXSM::Vector3 followDirection;
        DXSM::Vector3 target;
        DXSM::Vector3 up;
        DXSM::Vector3 forward;
        DXSM::Vector3 right;

        DXSM::Vector3 GetSpringArmRootOffset();
        void SetSpringArmRootOffset(DXSM::Vector3 newRootOffset);

        DXSM::Vector3 GetSpringArmRotation();
        void SetSpringArmRotation(DXSM::Vector3 newRotation);

        float GetSpringArmLength() { return m_springArmParams.length; };
        void ZoomSpringArm(float zoomSpeed = 1.0f);
        void SetSpringArmLength(float newLen) { m_springArmParams.length = fmin(fmax(0.0f, newLen), 1000.0f); };

        void RotateSpringArmYaw(float yawSpeed);
        void RotateSpringArmPitch(float pitchSpeed);
        void RollSpringArm(float rollSpeed);
        void RotateSpringArmYawPitch(float yawSpeed, float pitchSpeed);
        void RotateSpringArm(float yawSpeed, float pitchSpeed, float rollSpeed);

        DXSM::Vector3 GetCameraRotation();
        void SetCameraRotation(DXSM::Vector3 newRotation);

        void RotateCameraYaw(float yawSpeed);
        void RotateCameraPitch(float pitchSpeed);
        void RollCamera(float rollSpeed);
        void RotateCameraYawPitch(float yawSpeed, float pitchSpeed);
        void RotateCamera(float yawSpeed, float pitchSpeed, float rollSpeed);

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

        /*
        union {
            Scene* asScene;
            Scene_Info* asInfo;
        } m_scene;

        void AssignScene(Scene* scene);
        void AssignScene(Scene_Info* scene);
        */

        TransformComponent* m_assignedTransform = nullptr;

        SE::UUID m_assignedUUID = SE::UUID(0u);

    };
}
