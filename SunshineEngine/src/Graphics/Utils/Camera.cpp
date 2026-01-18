#include "Graphics/Utils/Camera.h"
#include <iostream>

#include <Scene.h>
#include <PlayerObject/PlayerObject.h>


namespace SE_G {
    Camera::Camera(ID3D11Device* device) : Camera(device, 1.0f) {
        InitBuffer(device);
    }

    Camera::Camera(ID3D11Device* device, float aspectRatio)
        : position(0.0f, 0.0f, -5.0f), target(0.0f, 0.0f, 1.0f), up(0.0f, 1.0f, 0.0f),
        aspectRatio(aspectRatio), nearZ(0.1f), farZ(1000.0f),
        orthZ(10.0f), isPerspective(true),
        cameraMode(CAMERA_MODE::FPS), orbitalTarget(0.0f, 0.0f, 0.0f),
        orbitalDistance(5.0f), minOrbitalDistance(5.0f),
        orbitalPitch(0.0f), orbitalYaw(0.0f),
        spinAxis(0.0f, 1.0f, 0.0f), orbitalAngleSpeed(0.0f)
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
            device, CameraBuffer{ viewProjMat, GetPosition(), 1.0f }, 1u);
    }

    void Camera::UpdateBuffer(ID3D11DeviceContext* context)
    {
        DXSM::Matrix viewProjMat = GetViewMatrix() * GetProjectionMatrix();
        cameraBuffer->Update(context, { viewProjMat, GetPosition(), 1.0f });
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
            DXSM::Matrix targetTransform = m_playerTransform->GetWorldMatrix();;

            DXSM::Vector3 targetPos;
            targetPos.x = targetTransform._41;
            targetPos.y = targetTransform._42;
            targetPos.z = targetTransform._43;

            Update(targetPos);
        }
    }

    void Camera::Update(const DXSM::Vector3 targetPoistion)
    {
        if (cameraMode == CAMERA_MODE::FOLLOW)
        {
            stickDirection = DXSM::Vector3(
                cosf(m_stickParams.stickPitch) * sinf(m_stickParams.stickYaw),
                sinf(m_stickParams.stickPitch),
                cosf(m_stickParams.stickPitch) * cosf(m_stickParams.stickYaw));
            
            DXSM::Vector3 _camPos = -m_stickParams.stickLength * stickDirection;
            
            position = targetPoistion + _camPos;

            right = DXSM::Vector3(sinf(m_stickParams.stickYaw + DX::XM_PIDIV2), 0.0f, cosf(m_stickParams.stickYaw + DX::XM_PIDIV2));
            up = DXSM::Vector3(0.0f, 1.0f, 0.0f);
            forward = DXSM::Vector3(sinf(m_stickParams.stickYaw), 0.0f, cosf(m_stickParams.stickYaw));
            
            /*
            rotateCamToForward = DXSM::Matrix::CreateFromYawPitchRoll(
                -m_stickParams.stickYaw,
                -m_stickParams.stickPitch,
                0.0f);

            _up = DXSM::Vector3::Transform(_up, rotateCamToForward);
            _forward = DXSM::Vector3::Transform(_forward, rotateCamToForward);
            _right = DXSM::Vector3::Transform(_right, rotateCamToForward);
            */

            position = position
                + right * m_stickParams.offset.x
                + up * m_stickParams.offset.y
                + forward * m_stickParams.offset.z;
            
            target = position - _camPos;

            /*
            DXSM::Vector3 stickVector(
                -1.0f * sinf(m_stickParams.stickYaw) * cosf(m_stickParams.stickPitch),
                1.0f * sinf(m_stickParams.stickPitch),
                -1.0f * cosf(m_stickParams.stickYaw) * cosf(m_stickParams.stickPitch)
            );

            stickVector *= m_stickParams.stickLength;

            position = targetPoistion + stickVector + m_stickParams.offset;

            DXSM::Vector3 _forward(0.0f, 0.0f, 1.0f);
            DXSM::Vector3 _up(0.0f, 1.0f, 0.0f);
            DXSM::Vector3 _right(1.0f, 0.0f, 0.0f);
            rotateCamToForward = DXSM::Matrix::CreateFromYawPitchRoll(
                m_stickParams.viewPitchYawRoll.y,
                -m_stickParams.viewPitchYawRoll.x,
                -m_stickParams.viewPitchYawRoll.z);
            up = DXSM::Vector3::Transform(_up, rotateCamToForward);
            forward = DXSM::Vector3::Transform(_forward, rotateCamToForward);
            right = DXSM::Vector3::Transform(_right, rotateCamToForward);

            target = position + _forward;
            */
        }
    }

    void Camera::Update(const DXSM::Matrix targetTransform)
    {
        if (cameraMode == CAMERA_MODE::ORBITAL)
        {
            //orbitalYaw += orbitalAngleSpeed * deltaTime;

            position.x = orbitalDistance * cosf(orbitalPitch) * cosf(orbitalYaw);
            position.y = orbitalDistance * sinf(orbitalPitch);
            position.z = orbitalDistance * cosf(orbitalPitch) * sinf(orbitalYaw);

            position = DXSM::Vector3::Transform(position, targetTransform);

            orbitalTarget = DXSM::Vector3::Transform(DXSM::Vector3::Zero, targetTransform);
            target = orbitalTarget;

            up = DXSM::Vector3::Transform(spinAxis, targetTransform) - orbitalTarget;
        }
        else if (cameraMode == CAMERA_MODE::FOLLOW)
        {
            //orbitalYaw += orbitalAngleSpeed * deltaTime;

            target = DXSM::Vector3::Transform(DXSM::Vector3::Zero, targetTransform);

            float cam2targetDist = 2.0f * referenceLen / tanf(fov * 0.5);
            position = target - cam2targetDist * (this->followDirection + sinf(followPitch) * up);
        }
    }

    void Camera::Update(const DXSM::Matrix targetTransform, DXSM::Vector3 direction)
    {
        if (cameraMode == CAMERA_MODE::FOLLOW)
        {
            //orbitalYaw += orbitalAngleSpeed * deltaTime;

            target = DXSM::Vector3::Transform(DXSM::Vector3::Zero, targetTransform);

            float cam2targetDist = 2.0f * referenceLen / tanf(fov * 0.5);
            position = target - cam2targetDist * (direction + sinf(followPitch) * up);
        }
    }

    void Camera::Update(const DXSM::Matrix targetTransform, DXSM::Vector3 direction, float referenceLen)
    {
        this->referenceLen = referenceLen;
        if (cameraMode == CAMERA_MODE::FOLLOW)
        {
            //orbitalYaw += orbitalAngleSpeed * deltaTime;

            target = DXSM::Vector3::Transform(DXSM::Vector3::Zero, targetTransform);

            float cam2targetDist = 2.0f * referenceLen / tanf(fov * 0.5);
            position = target - cam2targetDist * (direction + sinf(followPitch) * up);
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
        if (cameraMode == CAMERA_MODE::ORBITAL)
        {
            orbitalDistance = eastl::max(orbitalDistance - speed * m_deltaTime, referenceLen);
        }
        else
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
        }
        if (!isPerspective)
        {
            orthZ = eastl::max(orthZ + speed * m_deltaTime, nearZ * 1.1f);
        }
    }

    void Camera::MoveBackward(float speed)
    {
        MoveForward(-speed);
    }

    void Camera::MoveLeft(float speed)
    {
        if (cameraMode == CAMERA_MODE::ORBITAL)
        {
            orbitalYaw -= speed * m_deltaTime;
        }
        else
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
    }

    void Camera::MoveRight(float speed)
    {
        MoveLeft(-speed);
    }

    void Camera::MoveUp(float speed)
    {
        if (cameraMode == CAMERA_MODE::ORBITAL)
        {
            orbitalPitch += speed * m_deltaTime;
        }
        else
        {
            position.y += speed * m_deltaTime;
            target.y += speed * m_deltaTime;
        }
    }

    void Camera::MoveDown(float speed)
    {
        MoveUp(-speed);
    }

    void Camera::RotateYaw(float angle)
    {
        if (cameraMode == CAMERA_MODE::ORBITAL)
        {
            orbitalYaw += angle * m_deltaTime;
        }
        else
        {
            DXSM::Vector3 look_dir = DXSM::Vector3::Transform(target - position,
                DXSM::Matrix::CreateFromQuaternion(DXSM::Quaternion::CreateFromAxisAngle(up, angle * m_deltaTime)));
            target = position + look_dir;
        }
    }

    void Camera::RotatePitch(float angle)
    {
        if (cameraMode == CAMERA_MODE::ORBITAL)
        {
            orbitalPitch += angle * m_deltaTime;
        }
        else if (cameraMode == CAMERA_MODE::FOLLOW)
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

        m_stickParams.stickLength = 10.0f;
        m_stickParams.stickPitch = DX::XM_PI * 0.166f;
        m_stickParams.stickYaw = 0.0f;

        //m_stickParams.viewPitchYawRoll = { -DX::XM_PI * 0.166f, 0.0f, 0.0f };
        m_stickParams.viewPitchYawRoll = { 0.0f, 0.0f, 0.0f };

        m_stickParams.offset = DXSM::Vector3::Zero;
        /*
        followPitch = -DX::XM_PI * 0.166f;
        this->referenceLen = referenceLen;
        up = DXSM::Vector3(0.0f, 1.0f, 0.0f);
        */
    }

    void Camera::SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget)
    {
        SwitchToOrbitalMode(orbitalTarget, DXSM::Vector3(0.0f, 1.0f, 0.0f), 1.0f);
    }

    void Camera::SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget, DXSM::Vector3 spinAxis)
    {
        SwitchToOrbitalMode(orbitalTarget, spinAxis, 1.0f);
    }
    
    void Camera::SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget, DXSM::Vector3 spinAxis, float referenceLen)
    {
        this->referenceLen = referenceLen;
        cameraMode = CAMERA_MODE::ORBITAL;
        orbitalAngleSpeed = 0.0f;
        orbitalDistance = 2.0f * referenceLen / tanf(fov * 0.5);
        minOrbitalDistance = orbitalDistance;
        orthZ = 2.0f * referenceLen / tanf(fov * 0.5);
        orbitalYaw = 0.0f;
        orbitalPitch = DX::XM_PIDIV4;
        this->orbitalTarget = orbitalTarget;
        target = orbitalTarget;
        this->spinAxis = spinAxis;
        up = spinAxis;
        //orbitalDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&position), XMLoadFloat3(&target))));
    }

    void Camera::SwitchProjection() {
        isPerspective = !isPerspective;
        if (cameraMode == CAMERA_MODE::ORBITAL)
        {
            if (isPerspective)
                orbitalDistance = orthZ;
            else
            {
                orthZ = orbitalDistance;
                SetViewWidth(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ);
                SetViewHeight(2.0f * tanf(0.5f * fov) * orthZ);
            }
        }
        else if (cameraMode == CAMERA_MODE::FPS)
        {
            if (isPerspective)
                orbitalDistance = orthZ;
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

    void Camera::RotateStickYawPitch(float yawSpeed, float pitchSpeed)
    {
        float deltaYaw = yawSpeed * m_deltaTime;
        float deltaPitch = pitchSpeed * m_deltaTime;

        float _stickYaw = m_stickParams.stickYaw + deltaYaw;
        float _stickPitch = m_stickParams.stickPitch + deltaPitch;

        _stickPitch = fmax(-80.0f * DX::XM_PIDIV2 / 90.0f, fmin(_stickPitch, 80.0f * DX::XM_PIDIV2 / 90.0f));
        //deltaPitch = _stickPitch - m_stickParams.stickPitch;

        m_stickParams.stickPitch = _stickPitch;
        //m_stickParams.viewPitchYawRoll.x -= deltaPitch;

        //deltaYaw = _stickYaw - m_stickParams.stickYaw;
        m_stickParams.stickYaw = _stickYaw;
        //m_stickParams.viewPitchYawRoll.y += deltaYaw;

        // _stickYaw = _stickYaw > DX::XM_PI ? (_stickYaw - DX::XM_2PI) : _stickYaw;
        // _stickYaw = _stickYaw < -DX::XM_PI ? (_stickYaw + DX::XM_2PI) : _stickYaw;

        
    }

    DXSM::Vector3 Camera::GetStickDirection()
    {
        return stickDirection;
    }
}