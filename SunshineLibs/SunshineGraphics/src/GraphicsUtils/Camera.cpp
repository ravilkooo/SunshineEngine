

#include "GraphicsUtils/Camera.h"
#include <iostream>


Camera::Camera() : Camera(1.0f) {

}

Camera::Camera(float aspectRatio)
    : position(0.0f, 0.0f, -5.0f), target(0.0f, 0.0f, 1.0f), up(0.0f, 1.0f, 0.0f),
    fov(XM_PIDIV4), aspectRatio(aspectRatio), nearZ(0.1f), farZ(1000.0f),
    orthZ(10.0f), isPerspective(true),
    cameraMode(CAMERA_MODE::FPS), orbitalTarget(0.0f, 0.0f, 0.0f),
    orbitalDistance(5.0f), minOrbitalDistance(5.0f),
    orbitalPitch(0.0f), orbitalYaw(0.0f),
    spinAxis(0.0f, 1.0f, 0.0f), orbitalAngleSpeed(0.0f)
{

}

Camera::~Camera()
{
}

void Camera::SetPosition(Vector3 position)
{
    this->position = position;
}

Vector3 Camera::GetPosition()
{
    return this->position;
}

void Camera::SetTarget(Vector3 target)
{
    this->target = target;
}

Vector3 Camera::GetTarget()
{
    return target;
}

void Camera::SetUp(Vector3 up)
{
    this->up = up;
}

Vector3 Camera::GetUp()
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

void Camera::Update(float deltaTime, const Matrix targetTransform)
{
    if (cameraMode == CAMERA_MODE::ORBITAL)
    {
        //orbitalYaw += orbitalAngleSpeed * deltaTime;

        position.x = orbitalDistance * cosf(orbitalPitch) * cosf(orbitalYaw);
        position.y = orbitalDistance * sinf(orbitalPitch);
        position.z = orbitalDistance * cosf(orbitalPitch) * sinf(orbitalYaw);

        position = Vector3::Transform(position, targetTransform);

        orbitalTarget = Vector3::Transform(Vector3::Zero, targetTransform);
        target = orbitalTarget;

        up = Vector3::Transform(spinAxis, targetTransform) - orbitalTarget;
    }
}

void Camera::Update(float deltaTime, const Matrix targetTransform, Vector3 direction)
{
    if (cameraMode == CAMERA_MODE::FOLLOW)
    {
        //orbitalYaw += orbitalAngleSpeed * deltaTime;

        target = Vector3::Transform(Vector3::Zero, targetTransform);
        
        float cam2targetDist = 2.0f * referenceLen / tanf(fov * 0.5);
        position = target - cam2targetDist * (direction + sinf(followPitch) * up);
    }
}

void Camera::Update(float deltaTime, const Matrix targetTransform, Vector3 direction, float referenceLen)
{
    this->referenceLen = referenceLen;
    if (cameraMode == CAMERA_MODE::FOLLOW)
    {
        //orbitalYaw += orbitalAngleSpeed * deltaTime;

        target = Vector3::Transform(Vector3::Zero, targetTransform);

        float cam2targetDist = 2.0f * referenceLen / tanf(fov * 0.5);
        position = target - cam2targetDist * (direction + sinf(followPitch) * up);
    }
}

XMMATRIX Camera::GetViewMatrix() const
{
    return XMMatrixLookAtLH(position, target, up);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
    if (isPerspective)
        return XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
    else
        return XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
}

void Camera::MoveForward(float speed)
{
    if (cameraMode == CAMERA_MODE::ORBITAL)
    {
        orbitalDistance = eastl::max(orbitalDistance - speed, referenceLen);
    }
    else
    {
        //XMVECTOR forward = XMVectorSubtract(XMLoadFloat3(&target), XMLoadFloat3(&position));
        Vector3 forward = target - position;
        forward.Normalize();
        position.x += speed * forward.x;
        position.y += speed * forward.y;
        position.z += speed * forward.z;
        target.x += speed * forward.x;
        target.y += speed * forward.y;
        target.z += speed * forward.z;
    }
    if (!isPerspective)
    {
        orthZ = eastl::max(orthZ + speed, nearZ*1.1f);
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
        orbitalYaw -= speed;
    }
    else
    {
        /*
        XMVECTOR right = XMVector3Cross(
            XMVectorSubtract(XMLoadFloat3(&target), XMLoadFloat3(&position)),
            MLoadFloat3(&up)
        );
        */
        Vector3 right = (target - position).Cross(up);
        right.Normalize();
        position.x += speed * right.x;
        position.y += speed * right.y;
        position.z += speed * right.z;
        target.x += speed * right.x;
        target.y += speed * right.y;
        target.z += speed * right.z;
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
        orbitalPitch += speed;
    }
    else
    {
        position.y += speed;
        target.y += speed;
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
        orbitalYaw += angle;
    }
    else
    {
        Vector3 look_dir = Vector3::Transform(target - position,
            Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(up, angle)));
        target = position + look_dir;
    }
}

void Camera::RotatePitch(float angle)
{
    if (cameraMode == CAMERA_MODE::ORBITAL)
    {
        orbitalPitch += angle;
    }
    else if (cameraMode == CAMERA_MODE::FOLLOW)
    {
        followPitch = eastl::min(eastl::max(-XM_PIDIV2 * 0.9f, followPitch + angle), 0.0f);
        float cam2targetDist = 2.0f * referenceLen / tanf(fov * 0.5);
        Vector3 direction = (target - position);
        direction.y = 0; direction.Normalize();
        position = target - cam2targetDist * (direction + sinf(followPitch) * up);
    }
    else
    {
        Vector3 look_dir = target - position;
        Vector3 _axis = up.Cross(look_dir);
        look_dir = Vector3::Transform(look_dir,
            Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(_axis, -angle)));
        target = position + look_dir;
    }
}

void Camera::SwitchToFPSMode()
{
    cameraMode = CAMERA_MODE::FPS;
}

void Camera::SwitchToFollowMode(Vector3 followTarget, Vector3 direction, float referenceLen)
{
    cameraMode = CAMERA_MODE::FOLLOW;
    followPitch = -XM_PI * 0.166f;
    target = followTarget;
    this->referenceLen = referenceLen;
    up = Vector3(0.0f, 1.0f, 0.0f);
    float cam2targetDist = 2.0f * referenceLen / tanf(fov * 0.5);
    position = target - cam2targetDist * (direction + sinf(followPitch) * up);
}


void Camera::SwitchToOrbitalMode(Vector3 orbitalTarget)
{
    SwitchToOrbitalMode(orbitalTarget, Vector3(0.0f, 1.0f, 0.0f), 1.0f);
}

void Camera::SwitchToOrbitalMode(Vector3 orbitalTarget, Vector3 spinAxis)
{
    SwitchToOrbitalMode(orbitalTarget, spinAxis, 1.0f);
}
void Camera::SwitchToOrbitalMode(Vector3 orbitalTarget, Vector3 spinAxis, float referenceLen)
{
    this->referenceLen = referenceLen;
    cameraMode = CAMERA_MODE::ORBITAL;
    orbitalAngleSpeed = 0.0f;
    orbitalDistance = 2.0f * referenceLen / tanf(fov * 0.5);
    minOrbitalDistance = orbitalDistance;
    orthZ = 2.0f * referenceLen / tanf(fov * 0.5);
    orbitalYaw = 0.0f;
    orbitalPitch = XM_PIDIV4;
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
    Matrix mat = GetViewMatrix() * GetProjectionMatrix();
    FrustumPlanes planes;

    // Левая плоскость
    planes.Left = XMVectorSet(mat._14 + mat._11, mat._24 + mat._21, mat._34 + mat._31, mat._44 + mat._41);
    planes.Left = DirectX::XMPlaneNormalize(planes.Left);

    // Правая плоскость
    planes.Right = XMVectorSet(mat._14 - mat._11, mat._24 - mat._21, mat._34 - mat._31, mat._44 - mat._41);
    planes.Right = DirectX::XMPlaneNormalize(planes.Right);

    // Верхняя плоскость
    planes.Top = XMVectorSet(mat._14 - mat._12, mat._24 - mat._22, mat._34 - mat._32, mat._44 - mat._42);
    planes.Top = DirectX::XMPlaneNormalize(planes.Top);

    // Нижняя плоскость
    planes.Bottom = XMVectorSet(mat._14 + mat._12, mat._24 + mat._22, mat._34 + mat._32, mat._44 + mat._42);
    planes.Bottom = DirectX::XMPlaneNormalize(planes.Bottom);

    // Ближняя плоскость
    planes.Near = XMVectorSet(mat._14 + mat._13, mat._24 + mat._23, mat._34 + mat._33, mat._44 + mat._43);
    planes.Near = DirectX::XMPlaneNormalize(planes.Near);

    // Дальняя плоскость
    planes.Far = XMVectorSet(mat._14 - mat._13, mat._24 - mat._23, mat._34 - mat._33, mat._44 - mat._43);
    planes.Far = DirectX::XMPlaneNormalize(planes.Far);

    return planes;
}

Camera::FrustumCorners Camera::GetFrustumCorners()
{
    FrustumCorners corners;
    
    Matrix viewProjMatrix = GetViewMatrix() * GetProjectionMatrix();
    XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProjMatrix);

    XMVECTOR ndcCorners[8] = {
        XMVectorSet(-1, -1, 0, 1),
        XMVectorSet(1, -1, 0, 1),
        XMVectorSet(-1, 1, 0, 1),
        XMVectorSet(1, 1, 0, 1),
        XMVectorSet(-1, -1, 1, 1),
        XMVectorSet(1, -1, 1, 1),
        XMVectorSet(-1, 1, 1, 1),
        XMVectorSet(1, 1, 1, 1)
    };

    for (int i = 0; i < 8; ++i) {
        XMVECTOR worldPos = XMVector3TransformCoord(ndcCorners[i], invViewProj);
        if (i < 4) {
            corners.Near[i] = worldPos;
        }
        else {
            corners.Far[i - 4] = worldPos;
        }
    }

    return corners;
}
