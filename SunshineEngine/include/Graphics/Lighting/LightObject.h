#pragma once

#include <GameObject.h>

namespace SE_G {
    template <class T>
    class LightObject : public GameObject
    {
    public:
        eastl::shared_ptr<T> m_lightData;
        // Unnecessary?
        //virtual void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) = 0;

    };

    template <class T>
    class LightObject_Info : public GameObject_Info
    {
    public:
        eastl::shared_ptr<T> m_lightData;
        // Unnecessary?
        //virtual void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) = 0;

    };
}