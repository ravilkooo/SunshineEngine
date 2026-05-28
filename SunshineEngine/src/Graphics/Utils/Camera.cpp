#include "Graphics/Utils/Camera.h"
#include <iostream>

#include <Scene.h>
#include <PlayerObject/PlayerObject.h>
#include <Component/TransformComponent.h>

namespace SE_G {
    Camera::Camera(ID3D11Device* device) : Camera(device, 1.0f) {
        InitBuffer(device);
    }

    Camera::Camera(ID3D11Device* device, float aspectRatio)
        : position(0.0f, 0.0f, -5.0f), target(0.0f, 0.0f, 1.0f), up(0.0f, 1.0f, 0.0f),
        aspectRatio(aspectRatio), nearZ(0.1f), farZ(1000.0f),
        orthZ(10.0f), isPerspective(true),
        cameraMode(CAMERA_MODE::FPS)
    {
        //SetUpCameraViewByAspectRatio(aspectRatio);
        //fov = 2.0f * atan(aspectRatio * 0.5625f); 

        SetUpCameraViewByFOV(DX::XM_PI * 0.333f);

        InitBuffer(device);
    }

    Camera::~Camera()
    {

    }

    void Camera::InitBuffer(ID3D11Device* device)
    {
        //DXSM::Matrix viewProjMat = GetViewMatrix() * GetProjectionMatrix();
        DXSM::Matrix viewProjMat = GetViewMatrix() * GetProjectionMatrix();
        //CameraBuffer{ viewProjMat };
        cameraBuffer = eastl::make_unique<Bind::VertexConstantBuffer<CameraBuffer>>(
            device, CameraBuffer{ viewProjMat, GetPosition(), farZ }, 1u);
    }

    void Camera::UpdateBuffer(ID3D11DeviceContext* context)
    {
        DXSM::Matrix viewProjMat = GetViewMatrix() * GetProjectionMatrix();
        cameraBuffer->Update(context, { viewProjMat, GetPosition(), farZ });
    }

    void Camera::BindBuffer(ID3D11DeviceContext* context)
    {
        cameraBuffer->Bind(context);
    }

    void Camera::SetPosition(DXSM::Vector3 position)
    {
        this->position = position;
    }

    DXSM::Vector3 Camera::GetPosition()
    {
        return this->position;
    }

    void Camera::SetTarget(DXSM::Vector3 target)
    {
        this->target = target;
    }

    DXSM::Vector3 Camera::GetTarget()
    {
        return target;
    }

    void Camera::SetUp(DXSM::Vector3 up)
    {
        this->up = up;
    }

    DXSM::Vector3 Camera::GetUp()
    {
        return up;
    }

    DXSM::Vector3 Camera::GetRight()
    {
        return right;
    }

    DXSM::Vector3 Camera::GetForward()
    {
        return forward;
    }

    void Camera::SetFOV(float fov)
    {
        this->fov = fov;
    }

    void Camera::SetAspectRatio(float aspectRatio)
    {
        this->aspectRatio = aspectRatio;
    }

    void Camera::SetNearZ(float nearZ)
    {
        this->nearZ = nearZ;
    }

    float Camera::GetNearZ()
    {
        return nearZ;
    }

    void Camera::SetFarZ(float farZ)
    {
        this->farZ = farZ;
    }

    float Camera::GetFarZ()
    {
        return farZ;
    }

    void Camera::SetUpCameraViewByAspectRatio(float newAspectRatio)
    {
        SetAspectRatio(newAspectRatio);
        SetFOV(2.0f * atan(aspectRatio * 0.5625f)); // 2 * atan(9x/16)
        SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
        SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
    }

    void Camera::SetUpCameraViewByAspectRatio_horizontal(float newAspectRatio)
    {
        //SetFOV(fov); // stay the same
        SetAspectRatio(newAspectRatio);
        SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
        SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
    }

    void Camera::SetUpCameraViewByAspectRatio_vertical(float newAspectRatio)
    {
        SetFOV(2.0f * atan(tan(fov * 0.5f) * aspectRatio / newAspectRatio));
        SetAspectRatio(newAspectRatio);

        SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
        SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
    }

    void Camera::SetUpCameraViewByFOV(float newFOV)
    {
        SetFOV(newFOV);
        SetAspectRatio(16.0f * tan(fov * 0.5f) / 9.0f);
        SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
        SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
    }

    /*
    void Camera::SetUpCameraViewByFOV_horizontal(float newFOV)
    {
        SetFOV(newFOV);
        SetAspectRatio(16.0f * tan(fov * 0.5f) / 9.0f);
        SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
        SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
    }

    void Camera::SetUpCameraViewByFOV_vertical(float newFOV)
    {
        SetFOV(newFOV);
        SetAspectRatio(16.0f * tan(fov * 0.5f) / 9.0f);
        SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
        SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
    }
    */

    void Camera::ResetCameraView(float newAspectRatio)
    {
        SetFOV(DX::XM_PI * 0.333f);
        SetAspectRatio(newAspectRatio);
        SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
        SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);

        /*
        SetFOV(2.0f * atan(tan(DX::XM_PIDIV2 * 0.5f) * 16.0f / 9.0f / newAspectRatio));
        SetAspectRatio(newAspectRatio);
        //SetAspectRatio(newAspectRatio);
        //SetFOV(DX::XM_PIDIV2); // 2 * atan(1)
        SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
        SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
        */
    }

    void Camera::SetViewWidth(float viewWidth)
    {
        this->viewWidth = viewWidth;
    }

    float Camera::GetViewWidth()
    {
        return viewWidth;
    }

    void Camera::SetViewHeight(float viewHeight)
    {
        this->viewHeight = viewHeight;
    }

    float Camera::GetViewHeight()
    {
        return viewHeight;
    }

    void Camera::SetReferenceLen(float referenceLen)
    {
        this->referenceLen = eastl::min(eastl::max(0.2f, referenceLen), 100.0f);
    }

    float Camera::GetReferenceLen()
    {
        return referenceLen;
    }

    void Camera::Update(float deltaTime)
    {
		m_deltaTime = deltaTime;

        if (cameraMode == CAMERA_MODE::FOLLOW)
        {
            if (!m_playerTransform)
            {
                if (m_playerAsObject)
                {
                    auto pObj = m_scene.asScene->GetGameObjectByUUID(m_playerUUID);

                    m_playerTransform = pObj->GetComponent<TransformComponent>().get();
                }
                else
                {
                    auto pObj = m_scene.asInfo->GetGameObjectByUUID(m_playerUUID);
                    m_playerTransform = pObj->GetComponent<TransformComponent_Info>()->m_assignedComponent.get();
                }
            }
            DXSM::Matrix targetTransform = m_playerTransform->GetWorldMatrix_noLocal();

            DXSM::Vector3 targetPos;
            targetPos.x = targetTransform._41;
            targetPos.y = targetTransform._42;
            targetPos.z = targetTransform._43;

            UpdateTargetPoistion(targetPos);
        }
    }

    void Camera::UpdateTargetPoistion(const DXSM::Vector3 targetPoistion)
    {
        if (cameraMode == CAMERA_MODE::FOLLOW)
        {
            DXSM::Matrix cameraRot = DXSM::Matrix::CreateFromYawPitchRoll(
                cameraPitchYawRoll.y, cameraPitchYawRoll.x, cameraPitchYawRoll.z);

            right = DXSM::Vector3(1.0f, 0.0f, 0.0f); right = DXSM::Vector3::Transform(right, cameraRot);
            up = DXSM::Vector3(0.0f, 1.0f, 0.0f); up = DXSM::Vector3::Transform(up, cameraRot);
            forward = DXSM::Vector3(0.0f, 0.0f, 1.0f); forward = DXSM::Vector3::Transform(forward, cameraRot);

            DXSM::Vector3 final_position = m_springArmParams.rootOffset + DXSM::Vector3{ 0,0,-m_springArmParams.length };

            DXSM::Matrix springArmRot = DXSM::Matrix::CreateFromYawPitchRoll(
                m_springArmParams.pitchYawRoll.y, m_springArmParams.pitchYawRoll.x, m_springArmParams.pitchYawRoll.z);

            final_position = DXSM::Vector3::Transform(final_position, springArmRot);

            right = DXSM::Vector3::Transform(right, springArmRot);
            up = DXSM::Vector3::Transform(up, springArmRot);
            forward = DXSM::Vector3::Transform(forward, springArmRot);

            final_position = targetPoistion + final_position;
            position = final_position;
            
            target = position + forward;
        }
    }

    DX::XMMATRIX Camera::GetViewMatrix()
    {
        return XMMatrixLookAtLH(position, target, up);
    }

    DX::XMMATRIX Camera::GetProjectionMatrix() const
    {
        if (isPerspective)
            return DX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
        else
            return DX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
    }

    void Camera::MoveForward(float speed)
    {
        //DX::XMVECTOR forward = XMVectorSubtract(XMLoadFloat3(&target), XMLoadFloat3(&position));
        DXSM::Vector3 forward = target - position;
        forward.Normalize();
        position.x += speed * m_deltaTime * forward.x;
        position.y += speed * m_deltaTime * forward.y;
        position.z += speed * m_deltaTime * forward.z;
        target.x += speed * m_deltaTime * forward.x;
        target.y += speed * m_deltaTime * forward.y;
        target.z += speed * m_deltaTime * forward.z;
        if (!isPerspective)
        {
            // orthZ = eastl::max(orthZ + speed * m_deltaTime, nearZ * 1.1f);
        }
    }

    void Camera::MoveBackward(float speed)
    {
        MoveForward(-speed);
    }

    void Camera::MoveLeft(float speed)
    {
        /*
        DX::XMVECTOR right = XMVector3Cross(
            XMVectorSubtract(XMLoadFloat3(&target), XMLoadFloat3(&position)),
            MLoadFloat3(&up)
        );
        */
        DXSM::Vector3 right = (target - position).Cross(up);
        right.Normalize();
        position.x += speed * m_deltaTime * right.x;
        position.y += speed * m_deltaTime * right.y;
        position.z += speed * m_deltaTime * right.z;
        target.x += speed * m_deltaTime * right.x;
        target.y += speed * m_deltaTime * right.y;
        target.z += speed * m_deltaTime * right.z;
    }

    void Camera::MoveRight(float speed)
    {
        MoveLeft(-speed);
    }

    void Camera::MoveUp(float speed)
    {
        position.y += speed * m_deltaTime;
        target.y += speed * m_deltaTime;
    }

    void Camera::MoveDown(float speed)
    {
        MoveUp(-speed);
    }

    void Camera::RotateYaw(float angle)
    {
        DXSM::Vector3 look_dir = DXSM::Vector3::Transform(target - position,
            DXSM::Matrix::CreateFromQuaternion(DXSM::Quaternion::CreateFromAxisAngle(up, angle * m_deltaTime)));
        target = position + look_dir;
    }

    void Camera::RotatePitch(float angle)
    {
        if (cameraMode == CAMERA_MODE::FOLLOW)
        {
            followPitch = eastl::min(eastl::max(-DX::XM_PIDIV2 * 0.9f, followPitch + angle * m_deltaTime), 0.0f);
            float cam2targetDist = 2.0f * referenceLen / tanf(fov * 0.5);
            DXSM::Vector3 direction = (target - position);
            direction.y = 0; direction.Normalize();
            position = target - cam2targetDist * (direction + sinf(followPitch) * up);
        }
        else
        {
            DXSM::Vector3 look_dir = target - position;
            DXSM::Vector3 _axis = up.Cross(look_dir);
            look_dir = DXSM::Vector3::Transform(look_dir,
                DXSM::Matrix::CreateFromQuaternion(DXSM::Quaternion::CreateFromAxisAngle(_axis, -angle * m_deltaTime)));
            target = position + look_dir;
        }
    }

    void Camera::SwitchToFPSMode()
    {
        cameraMode = CAMERA_MODE::FPS;
    }

    void Camera::AssignScene(Scene* scene)
    {
        m_playerAsObject = true;
        m_scene.asScene = scene;
    }

    void Camera::AssignScene(Scene_Info* scene)
    {
        m_playerAsObject = false;
        m_scene.asInfo = scene;
    }

    void Camera::SetFollowPlayer(SE::UUID playerUUID)
    {
        m_playerUUID = playerUUID;
        InitFollowModeParams();
    }

    void Camera::InitFollowModeParams()
    {
        cameraMode = CAMERA_MODE::FOLLOW;

        m_springArmParams.length = 10.0f;
        m_springArmParams.pitchYawRoll = DXSM::Vector3( DX::XM_PI * 0.166f, 0.0f, 0.0f );

        m_springArmParams.rootOffset = DXSM::Vector3::Zero;
        /*
        followPitch = -DX::XM_PI * 0.166f;
        this->referenceLen = referenceLen;
        up = DXSM::Vector3(0.0f, 1.0f, 0.0f);
        */
    }

    void Camera::SwitchProjection() {
        isPerspective = !isPerspective;
        if (cameraMode == CAMERA_MODE::FPS)
        {
            if (isPerspective)
                ;
            else
            {
                orthZ = (position - target).Length();
                SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
                SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
            }
        }

    }

    Camera::FrustumPlanes Camera::GetFrustumPlanes()
    {
        DXSM::Matrix mat = GetViewMatrix() * GetProjectionMatrix();
        FrustumPlanes planes;

        // Левая плоскость
        planes.Left = DX::XMVectorSet(mat._14 + mat._11, mat._24 + mat._21, mat._34 + mat._31, mat._44 + mat._41);
        planes.Left = DirectX::XMPlaneNormalize(planes.Left);

        // Правая плоскость
        planes.Right = DX::XMVectorSet(mat._14 - mat._11, mat._24 - mat._21, mat._34 - mat._31, mat._44 - mat._41);
        planes.Right = DirectX::XMPlaneNormalize(planes.Right);

        // Верхняя плоскость
        planes.Top = DX::XMVectorSet(mat._14 - mat._12, mat._24 - mat._22, mat._34 - mat._32, mat._44 - mat._42);
        planes.Top = DirectX::XMPlaneNormalize(planes.Top);

        // Нижняя плоскость
        planes.Bottom = DX::XMVectorSet(mat._14 + mat._12, mat._24 + mat._22, mat._34 + mat._32, mat._44 + mat._42);
        planes.Bottom = DirectX::XMPlaneNormalize(planes.Bottom);

        // Ближняя плоскость
        planes.Near = DX::XMVectorSet(mat._14 + mat._13, mat._24 + mat._23, mat._34 + mat._33, mat._44 + mat._43);
        planes.Near = DirectX::XMPlaneNormalize(planes.Near);

        // Дальняя плоскость
        planes.Far = DX::XMVectorSet(mat._14 - mat._13, mat._24 - mat._23, mat._34 - mat._33, mat._44 - mat._43);
        planes.Far = DirectX::XMPlaneNormalize(planes.Far);

        return planes;
    }

    Camera::FrustumCorners Camera::GetFrustumCorners()
    {
        FrustumCorners corners;

        DXSM::Matrix viewProjMatrix = GetViewMatrix() * GetProjectionMatrix();
        DX::XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProjMatrix);

        DX::XMVECTOR ndcCorners[8] = {
            DX::XMVectorSet(-1, -1, 0, 1),
            DX::XMVectorSet(1, -1, 0, 1),
            DX::XMVectorSet(-1, 1, 0, 1),
            DX::XMVectorSet(1, 1, 0, 1),
            DX::XMVectorSet(-1, -1, 1, 1),
            DX::XMVectorSet(1, -1, 1, 1),
            DX::XMVectorSet(-1, 1, 1, 1),
            DX::XMVectorSet(1, 1, 1, 1)
        };

        for (int i = 0; i < 8; ++i) {
            DX::XMVECTOR worldPos = XMVector3TransformCoord(ndcCorners[i], invViewProj);
            if (i < 4) {
                corners.Near[i] = worldPos;
            }
            else {
                corners.Far[i - 4] = worldPos;
            }
        }

        return corners;
    }

    void Camera::RotateSpringArmYaw(float yawSpeed)
    {
        float deltaYaw = yawSpeed * m_deltaTime;

        float _stickYaw = m_springArmParams.pitchYawRoll.y + deltaYaw;
        _stickYaw = _stickYaw > DX::XM_PI ? (_stickYaw - DX::XM_2PI) : _stickYaw;
        _stickYaw = _stickYaw < -DX::XM_PI ? (_stickYaw + DX::XM_2PI) : _stickYaw;

        m_springArmParams.pitchYawRoll.y = _stickYaw;
    }

    void Camera::RotateSpringArmPitch(float pitchSpeed)
    {
        float deltaPitch = pitchSpeed * m_deltaTime;

        float _stickPitch = m_springArmParams.pitchYawRoll.x + deltaPitch;
        _stickPitch = fmax(-80.0f * DX::XM_PIDIV2 / 90.0f, fmin(_stickPitch, 80.0f * DX::XM_PIDIV2 / 90.0f));

        m_springArmParams.pitchYawRoll.x = _stickPitch;
    }

    void Camera::RollSpringArm(float rollSpeed)
    {
        float deltaRoll = rollSpeed * m_deltaTime;

        float _stickRoll = m_springArmParams.pitchYawRoll.z + deltaRoll;

        _stickRoll = _stickRoll > DX::XM_PI ? (_stickRoll - DX::XM_2PI) : _stickRoll;
        _stickRoll = _stickRoll < -DX::XM_PI ? (_stickRoll + DX::XM_2PI) : _stickRoll;

        m_springArmParams.pitchYawRoll.z = _stickRoll;
    }

    void Camera::RotateSpringArmYawPitch(float yawSpeed, float pitchSpeed)
    {
        RotateSpringArmYaw(yawSpeed);
        RotateSpringArmPitch(pitchSpeed);
    }

    void Camera::RotateSpringArm(float yawSpeed, float pitchSpeed, float rollSpeed)
    {
        RotateSpringArmYawPitch(yawSpeed, pitchSpeed);
        RollSpringArm(rollSpeed);
    }

    DXSM::Vector3 Camera::GetCameraRotation()
    {
        return cameraPitchYawRoll;
    }

    void Camera::SetCameraRotation(DXSM::Vector3 newRotation)
    {
        cameraPitchYawRoll = newRotation;
    }

    void Camera::RotateCameraYaw(float yawSpeed)
    {
        float deltaYaw = yawSpeed * m_deltaTime;

        float _cameraYaw = cameraPitchYawRoll.y + deltaYaw;
        _cameraYaw = _cameraYaw > DX::XM_PI ? (_cameraYaw - DX::XM_2PI) : _cameraYaw;
        _cameraYaw = _cameraYaw < -DX::XM_PI ? (_cameraYaw + DX::XM_2PI) : _cameraYaw;

        cameraPitchYawRoll.y = _cameraYaw;
    }

    void Camera::RotateCameraPitch(float pitchSpeed)
    {
        float deltaPitch = pitchSpeed * m_deltaTime;

        float _cameraPitch = cameraPitchYawRoll.x + deltaPitch;
        _cameraPitch = fmax(-80.0f * DX::XM_PIDIV2 / 90.0f, fmin(_cameraPitch, 80.0f * DX::XM_PIDIV2 / 90.0f));

        cameraPitchYawRoll.x = _cameraPitch;
    }

    void Camera::RollCamera(float rollSpeed)
    {
        float deltaRoll = rollSpeed * m_deltaTime;

        float _cameraRoll = cameraPitchYawRoll.z + deltaRoll;
        _cameraRoll = _cameraRoll > DX::XM_PI ? (_cameraRoll - DX::XM_2PI) : _cameraRoll;
        _cameraRoll = _cameraRoll < -DX::XM_PI ? (_cameraRoll + DX::XM_2PI) : _cameraRoll;

        cameraPitchYawRoll.z = _cameraRoll;
    }

    void Camera::RotateCameraYawPitch(float yawSpeed, float pitchSpeed)
    {
        RotateCameraYaw(yawSpeed);
        RotateCameraPitch(pitchSpeed);
    }

    void Camera::RotateCamera(float yawSpeed, float pitchSpeed, float rollSpeed)
    {
        RotateCameraYawPitch(yawSpeed, pitchSpeed);
        RollCamera(rollSpeed);
    }

    DXSM::Vector3 Camera::GetSpringArmRotation()
    {
        return m_springArmParams.pitchYawRoll;
    }

    void Camera::SetSpringArmRotation(DXSM::Vector3 newRotation)
    {
        m_springArmParams.pitchYawRoll = newRotation;
    }

    DXSM::Vector3 Camera::GetSpringArmRootOffset()
    {
        return m_springArmParams.rootOffset;
    }

    void Camera::SetSpringArmRootOffset(DXSM::Vector3 newRootOffset)
    {
        m_springArmParams.rootOffset = newRootOffset;
    }

    bool Camera::IsPerspectiveCamera()
    {
        return isPerspective;
	}
}