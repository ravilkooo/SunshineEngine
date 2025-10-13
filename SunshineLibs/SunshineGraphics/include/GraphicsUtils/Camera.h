#pragma once

#include <EASTL/algorithm.h>

#include <DirectXMath.h>
#include <SimpleMath.h>

#include "Bindable/ConstantBuffer.h"

using namespace DirectX;
namespace DXSM = DirectX::SimpleMath;

class Camera
{
public:
    struct CameraBuffer {
        DXSM::Matrix viewProjMatrix;
    };

    Bind::VertexConstantBuffer<CameraBuffer>* cameraBuffer;

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

    void Update(float deltaTime, const DXSM::Matrix targetTransform);
    void Update(float deltaTime, const DXSM::Matrix targetTransform, DXSM::Vector3 direction);
    void Update(float deltaTime, const DXSM::Matrix targetTransform, DXSM::Vector3 direction, float referenceLen);

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
    
    void SwitchToFollowMode(DXSM::Vector3 followTarget, DXSM::Vector3 direction, float referenceLen);

    void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget);
    void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget, DXSM::Vector3 spinAxis);
    void SwitchToOrbitalMode(DXSM::Vector3 orbitalTarget, DXSM::Vector3 spinAxis, float referenceLen);

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
    DXSM::Vector3 position;
    DXSM::Vector3 target;
    DXSM::Vector3 up;

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
};
