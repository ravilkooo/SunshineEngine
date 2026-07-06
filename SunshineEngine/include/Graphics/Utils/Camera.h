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

    private:
        enum class CAMERA_MODE
        {
            FPS, FOLLOW
        };

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

        bool m_isDirty = true;

        TransformComponent* m_assignedTransform = nullptr;

        SE::UUID m_assignedUUID = SE::UUID(0u);

        float m_deltaTime = 1.0f;

        void SetFOV(float newFov) { fov = newFov; }
        void SetAspectRatio(float newAspectRatio) { aspectRatio = newAspectRatio; }
        void SetViewWidth(float newViewWidth) { viewWidth = newViewWidth; }
        void SetViewHeight(float newViewHeight) { viewHeight = newViewHeight; }

    public:
        struct CameraBuffer {
            DXSM::Matrix viewProjMatrix;
            DXSM::Vector3 camPos;
            float pad;
        };

        eastl::unique_ptr<Bind::VertexConstantBuffer<CameraBuffer>> cameraBuffer;

        Camera(ID3D11Device* device);
        Camera(ID3D11Device* device, float aspectRatio);
        ~Camera();

        SE::UUID GetAssignedUUID() const { return m_assignedUUID; }

        void InitBuffer(ID3D11Device* device);
        void UpdateBuffer(ID3D11DeviceContext* context);
        void BindBuffer(ID3D11DeviceContext* context);

        bool IsDirty() const { return m_isDirty; };
        void MarkAsDirty() { m_isDirty = true; };

        const DXSM::Vector3& GetPosition() const { return position; }
        void SetPosition(const DXSM::Vector3& newPosition) { position = newPosition; }

        const DXSM::Vector3& GetTarget() const { return target; }
        void SetTarget(const DXSM::Vector3& newTarget) { target = newTarget; }

        const DXSM::Vector3& GetUp() const { return up; }
        void SetUp(const DXSM::Vector3& newUp) { up = newUp; }

        const DXSM::Vector3& GetRight() const { return right; }
        const DXSM::Vector3& GetForward() const { return forward; }

        float GetNearZ() const { return nearZ; }
        void SetNearZ(float newNearZ) { nearZ = newNearZ; }

        float GetFarZ() const { return farZ; }
        void SetFarZ(float newFarZ) { farZ = newFarZ; }

        void SetUpCameraViewByAspectRatio(float newAspectRatio);
        void SetUpCameraViewByAspectRatio_horizontal(float newAspectRatio);
        void SetUpCameraViewByAspectRatio_vertical(float newAspectRatio);
        void SetUpCameraViewByFOV(float newFOV);
        /*
        void SetUpCameraViewByFOV_horizontal(float newFOV);
        void SetUpCameraViewByFOV_vertical(float newFOV);
        */

        void ResetCameraView(float newAspectRatio);

        float GetViewWidth() const { return viewWidth; }
        float GetViewHeight() const { return viewHeight; }

        float GetReferenceLen() const { return referenceLen; }
        void SetReferenceLen(float referenceLen) { this->referenceLen = eastl::min(eastl::max(0.2f, referenceLen), 100.0f); }

        void Update(float deltaTime);
        void UpdateTargetPoistion(const DXSM::Vector3& targetPoistion);

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
        bool IsPerspectiveCamera() const { return isPerspective; }

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

        const DXSM::Vector3& GetSpringArmRootOffset() const { return m_springArmParams.rootOffset; }
        void SetSpringArmRootOffset(const DXSM::Vector3& newRootOffset) { m_springArmParams.rootOffset = newRootOffset; }

        const DXSM::Vector3& GetSpringArmRotation() const { return m_springArmParams.pitchYawRoll; }
        void SetSpringArmRotation(const DXSM::Vector3& newRotation) { m_springArmParams.pitchYawRoll = newRotation; }
        void SetSpringArmYaw(float newYaw) { m_springArmParams.pitchYawRoll.y = newYaw; }

        float GetSpringArmLength() const { return m_springArmParams.length; };
        void ZoomSpringArm(float zoomSpeed = 1.0f);
        void SetSpringArmLength(float newLen) { m_springArmParams.length = fmin(fmax(0.0f, newLen), 1000.0f); };

		float GetZoomAcceleration() const { return m_zoomAcceleration; }
		void SetZoomAcceleration(float newAcceleration) { m_zoomAcceleration = fmax(0.0f, newAcceleration); }

        void RotateSpringArmYaw(float yawSpeed);
        void RotateSpringArmPitch(float pitchSpeed);
        void RollSpringArm(float rollSpeed);
        void RotateSpringArmYawPitch(float yawSpeed, float pitchSpeed);
        void RotateSpringArm(float yawSpeed, float pitchSpeed, float rollSpeed);

        const DXSM::Vector3& GetCameraRotation() const { return cameraPitchYawRoll; }
        void SetCameraRotation(const DXSM::Vector3& newRotation) { cameraPitchYawRoll = newRotation; }

        void RotateCameraYaw(float yawSpeed);
        void RotateCameraPitch(float pitchSpeed);
        void RollCamera(float rollSpeed);
        void RotateCameraYawPitch(float yawSpeed, float pitchSpeed);
        void RotateCamera(float yawSpeed, float pitchSpeed, float rollSpeed);
    };
}
