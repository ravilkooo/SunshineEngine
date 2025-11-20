#pragma once

#include "RenderTechnique.h"
#include <Graphics/Bindable/ConstantBuffer.h>

#include <Physics/PhysicsEnums.h>

namespace SE_G {

    struct ColliderVertex {
        DXSM::Vector3 position = { 0.0f, 0.0f, 0.0f };
    };

    struct BufferOffset {
        UINT vertexStart;
        UINT vertexBufferSize;

        UINT indexStart;
        UINT indexBufferSize;
    };

    class ColliderTechnique :
        public RenderTechnique
    {
    public:
        ColliderTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
            eastl::string technique, SE::CollisionShape shape = SE::CollisionShape::Box);

        void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        SE::CollisionShape m_shape = SE::CollisionShape::Box;

        // To-do: implement custom mesh collider
        // void InitCustomColliderMesh();
        // eastl::unique_ptr<Bind::VertexBuffer> m_customVertexBuffer;

    private:
        static void InitStaticData(ID3D11Device* device);

        static bool s_staticDataInitializated;
        // every object need constantBuffer for settings
        // (xyz-size) + (?rotation ?offset)
        static eastl::unique_ptr<Bind::VertexShader> s_planesSymmetryShader;

        // every object need constantBuffer for settings
        // (upper, lower radius, height) + (?rotation ?offset)
        static eastl::unique_ptr<Bind::VertexShader> s_yAxizSymmetryShader;

        // every object need constantBuffer for transforming mesh
        // (at least scaling or full transform buffer, see local transform)
        static eastl::unique_ptr<Bind::VertexShader> s_customShader;

    };

}