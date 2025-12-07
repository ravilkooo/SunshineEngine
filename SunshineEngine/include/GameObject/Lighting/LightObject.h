#pragma once

#include <GameObject/GameObject.h>
#include <Serialization/LightDataSerialization.h>

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

    virtual json ToJson() const override {
        json j = GameObject_Info::ToJson();
        j["m_lightData"] = *(m_lightData.get());
        return j;
    }
};