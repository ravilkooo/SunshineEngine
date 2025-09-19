#pragma once

#include <EASTL/algorithm.h>

#include <DirectXMath.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;


class Camera
{
public:
    enum class CAMERA_MODE
    {
        FPS, ORBITAL, FOLLOW
    };

    Camera();
    Camera(float aspectRatio);
    ~Camera();

    void SetPosition(Vector3 position);
    Vector3 GetPosition();

    void SetTarget(Vector3 target);
    Vector3 GetTarget();

    void SetUp(Vector3 up);
    Vector3 GetUp();

    void SetFOV(float fov);
    void SetAspectRatio(float aspectRatio);

    void SetNearZ(float nearZ);
    float GetNearZ();

    void SetFarZ(float farZ);
    float GetFarZ();

    void SetViewWidth(float viewWidth);
    float GetViewWidth();

    void SetViewHeight(float viewHeight);
    float GetViewHeight();

    void SetReferenceLen(float referenceLen);
    float GetReferenceLen();

    void Update(float deltaTime, const Matrix targetTransform);
    void Update(float deltaTime, const Matrix targetTransform, Vector3 direction);
    void Update(float deltaTime, const Matrix targetTransform, Vector3 direction, float referenceLen);

    XMMATRIX GetViewMatrix() const;
    XMMATRIX GetProjectionMatrix() const;

    void MoveForward(float speed);
    void MoveBackward(float speed);
    void MoveLeft(float speed);
    void MoveRight(float speed);
    void MoveUp(float speed);
    void MoveDown(float speed);

    void RotateYaw(float angle);
    void RotatePitch(float angle);

    void SwitchToFPSMode();
    
    void SwitchToFollowMode(Vector3 followTarget, Vector3 direction, float referenceLen);

    void SwitchToOrbitalMode(Vector3 orbitalTarget);
    void SwitchToOrbitalMode(Vector3 orbitalTarget, Vector3 spinAxis);
    void SwitchToOrbitalMode(Vector3 orbitalTarget, Vector3 spinAxis, float referenceLen);

    void SwitchProjection();

    struct FrustumPlanes {
        XMVECTOR Left;
        XMVECTOR Right;
        XMVECTOR Top;
        XMVECTOR Bottom;
        XMVECTOR Near;
        XMVECTOR Far;
    };

    FrustumPlanes GetFrustumPlanes();

    struct FrustumCorners {
        XMVECTOR Near[4];
        XMVECTOR Far[4];
    };
    FrustumCorners GetFrustumCorners();

private:
    Vector3 position;
    Vector3 target;
    Vector3 up;

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
    Vector3 orbitalTarget;
    float minOrbitalDistance;
    float orbitalDistance;
    float orbitalYaw;
    float orbitalPitch;
    float orbitalAngleSpeed;
    //float orbitalAngleSpeed;
    Vector3 spinAxis;

    // for FOLLOW camera mode
    float followPitch;


};
