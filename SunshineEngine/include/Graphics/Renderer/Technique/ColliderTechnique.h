#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>

#include "RenderTechnique.h"
#include <Graphics/Bindable/ConstantBuffer.h>

#include <Physics/PhysicsEnums.h>
#include <Physics/CollisionUtils.h>

namespace SE_G {

    class ColliderTechnique :
        public RenderTechnique
    {
    public:
        ColliderTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
            eastl::string technique, eastl::shared_ptr<SE::ColliderData> colliderData);

        void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void UpdateTransformCB(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
        void UpdateSettingsCB(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    private:
        eastl::shared_ptr<SE::ColliderData> m_colliderData;

        eastl::unique_ptr<Bind::VertexConstantBuffer<SE::ColliderTransformCB>> m_transformCB;
        eastl::unique_ptr<Bind::VertexConstantBuffer<SE::ColliderSettings>> m_settingsCB;

        // To-do: implement custom mesh collider
        // void InitCustomColliderMesh();
        // eastl::unique_ptr<Bind::VertexBuffer> m_customVertexBuffer;

    private:
        static void InitStaticData(ID3D11Device* device);

        static bool s_staticDataInitializated;
        // every object need constantBuffer for settings
        
        // (radius)
        static eastl::unique_ptr<Bind::VertexShader> s_sphereShader;
        
        // (xyz-size)
        static eastl::unique_ptr<Bind::VertexShader> s_boxShader;

        // (upper, lower radius, height)
        static eastl::unique_ptr<Bind::VertexShader> s_capsuleShader;

        // (upper, lower radius, height)
        static eastl::unique_ptr<Bind::VertexShader> s_taperedCapsuleShader;

        // (at least scaling or full transform buffer, see local transform)
        static eastl::unique_ptr<Bind::VertexShader> s_customShader;

    };

}