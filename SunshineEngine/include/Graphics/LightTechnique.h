#pragma once
#include "RenderTechnique.h"
#include <Bindable/ConstantBuffer.h>
#include "GraphicsUtils/Camera.h"
#include "Graphics/Lighting/LightData.h"

enum class LightPosition {
    INSIDE, INTERSECT_FAR_PLANE, OUTSIDE, FILL, BEHIND_NEAR_PLANE
};

template <class T>
class LightTechnique :
    public RenderTechnique
{
public:
    eastl::shared_ptr<T> lightData;
    eastl::shared_ptr<Bind::PixelConstantBuffer<T>> lightDataBuffer;
    eastl::shared_ptr<Camera> m_camera;

    LightTechnique(ID3D11Device* device, eastl::string technique);
    ~LightTechnique() = default;

    void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

    // Update right before draw
    // Need camera?
    virtual LightPosition GetLightPositionInFrustum() { return LightPosition::FILL; };
    virtual bool IsFrustumInsideOfLight() { return true; };

    virtual void ChooseDepthStencilState(LightPosition lightPos) = 0;
    virtual void ChooseRasterizer(LightPosition lightPos) = 0;
};

template class LightTechnique<AmbientLightData>;
template void LightTechnique<AmbientLightData>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);

template class LightTechnique<DirectionalLightData>;
template void LightTechnique<DirectionalLightData>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);

template class LightTechnique<PointLightData>;
template void LightTechnique<PointLightData>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);

template class LightTechnique<SkyBoxData>;
template void LightTechnique<SkyBoxData>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);
