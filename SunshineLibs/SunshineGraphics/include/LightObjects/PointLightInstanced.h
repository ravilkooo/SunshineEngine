#pragma once



#include "LightObject.h"
#include "PointLight.h"

class PointLightInstanced :
    public LightObject
{
public:
    //PointLight::PointLightPCB* lightsData;
    UINT lightsCnt;
    Bind::VertexBuffer* instanceBuffer;
    
    struct PointLightMovement
    {
        XMFLOAT3 Velocity;
        float pad;
        XMFLOAT3 Acceleration;
        float pad2;
    };

    PointLightInstanced();
    PointLightInstanced(ID3D11Device* device, UINT lightsCnt,
        PointLight::PointLightPCB* initLightsData, PointLightMovement* movementData,
        Vector4 ambient, float range);

    Vector4 ambient;

    D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc(LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum(Camera* camera) override;
    bool IsFrustumInsideOfLight(Camera* camera) override;

    void DrawTechnique(std::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept override;
    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;
    void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

    // Compute shader stuff
    void SetUpdateComputeShader(ID3D11Device* device, LPCWSTR filePath);
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pUpdatePointLightDataComputeShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> pPointLightData;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pPointLightDataUAV;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pMovementBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pMovementBufferUAV;
    
    struct PointLightInstancedCB
    {
        float dt;
        float rngSeed;
        UINT pointLightCount;
        float padding;
    } pliConstBufferData;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pPointLightConstBufferData;
};

